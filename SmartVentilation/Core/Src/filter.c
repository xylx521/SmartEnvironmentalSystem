/**
  ******************************************************************************
  * @file           : filter.c
  * @brief          : 数据滤波算法实现
  * @author         : Your Name
  * @date           : 2025-12-02
  ******************************************************************************
  */

#include "filter.h"
#include <string.h>

/* ============================================================================
   滑动平均滤波器 (Moving Average Filter)
   ============================================================================ */

/**
  * @brief  初始化滑动平均滤波器
  * @param  filter: 滤波器结构体指针
  * @retval None
  */
void MovingAvg_Init(MovingAvgFilter_t *filter)
{
    memset(filter->buffer, 0, sizeof(filter->buffer));
    filter->index = 0;
    filter->count = 0;
    filter->sum = 0;
}

/**
  * @brief  滑动平均滤波更新
  * @param  filter: 滤波器结构体指针
  * @param  new_value: 新采样值
  * @retval 滤波后的值
  * @note   使用优化算法：sum增量更新，避免每次都遍历数组
  */
uint16_t MovingAvg_Update(MovingAvgFilter_t *filter, uint16_t new_value)
{
    /* 如果缓冲区已满，减去即将被替换的旧值 */
    if (filter->count >= MA_WINDOW_SIZE)
    {
        filter->sum -= filter->buffer[filter->index];
    }
    else
    {
        filter->count++;
    }
    
    /* 添加新值 */
    filter->buffer[filter->index] = new_value;
    filter->sum += new_value;
    
    /* 更新索引（循环缓冲区） */
    filter->index++;
    if (filter->index >= MA_WINDOW_SIZE)
    {
        filter->index = 0;
    }
    
    /* 返回平均值 */
    return (uint16_t)(filter->sum / filter->count);
}

/**
  * @brief  重置滑动平均滤波器
  * @param  filter: 滤波器结构体指针
  * @retval None
  */
void MovingAvg_Reset(MovingAvgFilter_t *filter)
{
    MovingAvg_Init(filter);
}

/* ============================================================================
   中位值滤波器 (Median Filter)
   ============================================================================ */

/**
  * @brief  初始化中位值滤波器
  * @param  filter: 滤波器结构体指针
  * @retval None
  */
void Median_Init(MedianFilter_t *filter)
{
    memset(filter->buffer, 0, sizeof(filter->buffer));
    filter->index = 0;
    filter->count = 0;
}

/**
  * @brief  冒泡排序（用于中位值滤波）
  * @param  arr: 数组
  * @param  len: 数组长度
  * @retval None
  */
static void BubbleSort(uint16_t arr[], uint8_t len)
{
    uint8_t i, j;
    uint16_t temp;
    
    for (i = 0; i < len - 1; i++)
    {
        for (j = 0; j < len - 1 - i; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

/**
  * @brief  中位值滤波更新
  * @param  filter: 滤波器结构体指针
  * @param  new_value: 新采样值
  * @retval 滤波后的值（中位数）
  * @note   对窗口内数据排序，取中间值，可有效去除脉冲干扰
  */
uint16_t Median_Update(MedianFilter_t *filter, uint16_t new_value)
{
    uint16_t sorted_buffer[MEDIAN_WINDOW_SIZE];
    uint8_t i;
    
    /* 添加新值到缓冲区 */
    filter->buffer[filter->index] = new_value;
    
    /* 更新索引 */
    filter->index++;
    if (filter->index >= MEDIAN_WINDOW_SIZE)
    {
        filter->index = 0;
    }
    
    /* 更新计数 */
    if (filter->count < MEDIAN_WINDOW_SIZE)
    {
        filter->count++;
    }
    
    /* 如果数据不足，返回平均值 */
    if (filter->count < MEDIAN_WINDOW_SIZE)
    {
        uint32_t sum = 0;
        for (i = 0; i < filter->count; i++)
        {
            sum += filter->buffer[i];
        }
        return (uint16_t)(sum / filter->count);
    }
    
    /* 复制数据到临时数组进行排序（不破坏原数据顺序） */
    memcpy(sorted_buffer, filter->buffer, sizeof(sorted_buffer));
    
    /* 排序 */
    BubbleSort(sorted_buffer, MEDIAN_WINDOW_SIZE);
    
    /* 返回中位数（中间值） */
    return sorted_buffer[MEDIAN_WINDOW_SIZE / 2];
}

/**
  * @brief  重置中位值滤波器
  * @param  filter: 滤波器结构体指针
  * @retval None
  */
void Median_Reset(MedianFilter_t *filter)
{
    Median_Init(filter);
}

/* ============================================================================
   一阶滞后滤波器 (First Order Lag Filter)
   ============================================================================ */

/**
  * @brief  初始化一阶滞后滤波器
  * @param  filter: 滤波器结构体指针
  * @param  alpha: 滤波系数 (0-1)
  *               - 接近0：滤波强，响应慢
  *               - 接近1：滤波弱，响应快
  *               - 推荐值：0.1 - 0.3
  * @retval None
  */
void FirstOrder_Init(FirstOrderFilter_t *filter, float alpha)
{
    filter->last_output = 0.0f;
    filter->alpha = alpha;
}

/**
  * @brief  一阶滞后滤波更新
  * @param  filter: 滤波器结构体指针
  * @param  new_value: 新采样值
  * @retval 滤波后的值
  * @note   公式：Y(n) = α × X(n) + (1-α) × Y(n-1)
  */
float FirstOrder_Update(FirstOrderFilter_t *filter, float new_value)
{
    filter->last_output = filter->alpha * new_value + 
                         (1.0f - filter->alpha) * filter->last_output;
    return filter->last_output;
}

/**
  * @brief  重置一阶滞后滤波器
  * @param  filter: 滤波器结构体指针
  * @retval None
  */
void FirstOrder_Reset(FirstOrderFilter_t *filter)
{
    filter->last_output = 0.0f;
}

/* ============================================================================
   组合滤波器 (Hybrid Filter)
   ============================================================================ */

/**
  * @brief  组合滤波：先中位值去除脉冲，再滑动平均平滑
  * @param  new_value: 新采样值
  * @param  ma_filter: 滑动平均滤波器指针
  * @param  median_filter: 中位值滤波器指针
  * @retval 滤波后的值
  * @note   这是推荐的滤波方案，效果最好
  */
uint16_t Hybrid_Filter(uint16_t new_value, 
                       MovingAvgFilter_t *ma_filter, 
                       MedianFilter_t *median_filter)
{
    uint16_t median_output;
    uint16_t final_output;
    
    /* 第一步：中位值滤波（去除脉冲干扰） */
    median_output = Median_Update(median_filter, new_value);
    
    /* 第二步：滑动平均滤波（平滑曲线） */
    final_output = MovingAvg_Update(ma_filter, median_output);
    
    return final_output;
}
