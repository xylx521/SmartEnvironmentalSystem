/**
  ******************************************************************************
  * @file           : adc_driver.c
  * @brief          : ADC驱动实现 - MQ气体传感器读取
  * @author         : Your Name
  * @date           : 2025-11-27
  ******************************************************************************
  */

#include "adc_driver.h"
#include "adc.h"
#include <math.h>

/* 外部ADC句柄 (由CubeMX生成在adc.c中) */
extern ADC_HandleTypeDef hadc1;

/* ADC参数定义 */
#define ADC_RESOLUTION      4096.0f     // 12位ADC分辨率
#define ADC_VREF            3.3f        // 参考电压(V)

/* MQ传感器校准参数 (需要根据实际传感器调整) */
#define MQ4_R0              10.0f       // MQ4在清洁空气中的阻值(kΩ)
#define MQ2_R0              10.0f       // MQ2在清洁空气中的阻值(kΩ)
#define MQ7_R0              10.0f       // MQ7在清洁空气中的阻值(kΩ)
#define RL_VALUE            10.0f       // 负载电阻(kΩ)

/**
  * @brief  初始化ADC驱动
  * @param  None
  * @retval None
  */
void ADC_Driver_Init(void)
{
    /* 启动ADC校准 */
    HAL_ADCEx_Calibration_Start(&hadc1);
    
    /* 启动ADC */
    HAL_ADC_Start(&hadc1);
}

/**
  * @brief  读取所有传感器ADC原始值
  * @param  raw_data: 存储原始ADC值的结构体指针
  * @retval None
  */
void ADC_ReadAllSensors(ADC_RawData_t *raw_data)
{
    raw_data->mq4_raw = ADC_ReadSingleChannel(SENSOR_MQ4);
    raw_data->mq2_raw = ADC_ReadSingleChannel(SENSOR_MQ2);
    raw_data->mq7_raw = ADC_ReadSingleChannel(SENSOR_MQ7);
}

/**
  * @brief  读取单个通道ADC值
  * @param  sensor: 传感器类型
  * @retval ADC原始值 (0-4095)
  */
uint16_t ADC_ReadSingleChannel(SensorType_t sensor)
{
    uint16_t adc_value = 0;
    
    /* 等待转换完成 */
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
    {
        /* 根据传感器序号读取对应的ADC值 */
        if (sensor < SENSOR_NUM)
        {
            adc_value = HAL_ADC_GetValue(&hadc1);
        }
    }
    
    return adc_value;
}

/**
  * @brief  将ADC值转换为电压
  * @param  adc_value: ADC原始值 (0-4095)
  * @retval 电压值 (V)
  */
float ADC_GetVoltage(uint16_t adc_value)
{
    return ((float)adc_value / ADC_RESOLUTION) * ADC_VREF;
}

/**
  * @brief  将ADC原始值转换为PPM浓度
  * @param  raw_data: 原始ADC数据
  * @param  sensor_data: 转换后的传感器浓度数据
  * @retval None
  * @note   这里使用简化的线性转换，实际应用需要根据传感器数据手册校准
  */
void ADC_ConvertToPPM(ADC_RawData_t *raw_data, SensorData_t *sensor_data)
{
    float voltage;
    float rs;  // 传感器电阻
    float ratio; // Rs/R0比值
    
    /* MQ4 - 甲烷传感器转换 */
    voltage = ADC_GetVoltage(raw_data->mq4_raw);
    if (voltage > 0.1f) // 避免除零
    {
        rs = ((ADC_VREF - voltage) / voltage) * RL_VALUE;
        ratio = rs / MQ4_R0;
        // 简化的对数转换公式 (实际需要根据数据手册曲线拟合)
        sensor_data->mq4_ppm = pow(10, ((log10(ratio) - 0.5) / -0.4)) * 200.0f;
    }
    else
    {
        sensor_data->mq4_ppm = 0.0f;
    }
    
    /* MQ2 - 烟雾传感器转换 */
    voltage = ADC_GetVoltage(raw_data->mq2_raw);
    if (voltage > 0.1f)
    {
        rs = ((ADC_VREF - voltage) / voltage) * RL_VALUE;
        ratio = rs / MQ2_R0;
        sensor_data->mq2_ppm = pow(10, ((log10(ratio) - 0.6) / -0.45)) * 300.0f;
    }
    else
    {
        sensor_data->mq2_ppm = 0.0f;
    }
    
    /* MQ7 - 一氧化碳传感器转换 */
    voltage = ADC_GetVoltage(raw_data->mq7_raw);
    if (voltage > 0.1f)
    {
        rs = ((ADC_VREF - voltage) / voltage) * RL_VALUE;
        ratio = rs / MQ7_R0;
        sensor_data->mq7_ppm = pow(10, ((log10(ratio) - 0.7) / -0.35)) * 100.0f;
    }
    else
    {
        sensor_data->mq7_ppm = 0.0f;
    }
}
