/**
  ******************************************************************************
  * @file           : filter.h
  * @brief          : 数据滤波算法头文件
  * @author         : xylx521
  * @date           : 2025-12-02
  ******************************************************************************
  */

#ifndef __FILTER_H
#define __FILTER_H

#include "main.h"

/* 滑动平均滤波器配置 */
#define MA_WINDOW_SIZE      10      // 滑动窗口大小（建议5-20）
#define MEDIAN_WINDOW_SIZE  5       // 中位值滤波窗口（必须是奇数）

/* 滑动平均滤波器结构体 */
typedef struct {
    uint16_t buffer[MA_WINDOW_SIZE];  // 数据缓冲区
    uint8_t  index;                    // 当前索引
    uint8_t  count;                    // 已存储数据个数
    uint32_t sum;                      // 数据总和（优化计算）
} MovingAvgFilter_t;

/* 中位值滤波器结构体 */
typedef struct {
    uint16_t buffer[MEDIAN_WINDOW_SIZE];  // 数据缓冲区
    uint8_t  index;                        // 当前索引
    uint8_t  count;                        // 已存储数据个数
} MedianFilter_t;

/* 一阶滞后滤波器结构体 */
typedef struct {
    float    last_output;    // 上次输出值
    float    alpha;          // 滤波系数 (0-1)，越小越平滑
} FirstOrderFilter_t;

/* 函数声明 - 滑动平均滤波 */
void MovingAvg_Init(MovingAvgFilter_t *filter);
uint16_t MovingAvg_Update(MovingAvgFilter_t *filter, uint16_t new_value);
void MovingAvg_Reset(MovingAvgFilter_t *filter);

/* 函数声明 - 中位值滤波 */
void Median_Init(MedianFilter_t *filter);
uint16_t Median_Update(MedianFilter_t *filter, uint16_t new_value);
void Median_Reset(MedianFilter_t *filter);

/* 函数声明 - 一阶滞后滤波 */
void FirstOrder_Init(FirstOrderFilter_t *filter, float alpha);
float FirstOrder_Update(FirstOrderFilter_t *filter, float new_value);
void FirstOrder_Reset(FirstOrderFilter_t *filter);

/* 组合滤波函数 */
uint16_t Hybrid_Filter(uint16_t new_value, MovingAvgFilter_t *ma_filter, MedianFilter_t *median_filter);

#endif /* __FILTER_H */
