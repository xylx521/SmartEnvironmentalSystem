/**
  ******************************************************************************
  * @file           : adc_driver.h
  * @brief          : ADC驱动头文件 - MQ气体传感器读取
  * @author         : xylx521
  * @date           : 2025-11-27
  ******************************************************************************
  */

#ifndef __ADC_DRIVER_H
#define __ADC_DRIVER_H

#include "main.h"
#include "filter.h"

/* 传感器通道定义 */
typedef enum {
    SENSOR_MQ4 = 0,   // 甲烷传感器 (ADC Channel 0)
    SENSOR_MQ2 = 1,   // 烟雾传感器 (ADC Channel 1)
    SENSOR_MQ7 = 2,   // 一氧化碳传感器 (ADC Channel 2)
    SENSOR_NUM = 3    // 传感器总数
} SensorType_t;

/* ADC原始数据结构 */
typedef struct {
    uint16_t mq4_raw;    // MQ4原始ADC值 (0-4095)
    uint16_t mq2_raw;    // MQ2原始ADC值 (0-4095)
    uint16_t mq7_raw;    // MQ7原始ADC值 (0-4095)
} ADC_RawData_t;

/* 传感器浓度数据结构 (ppm) */
typedef struct {
    float mq4_ppm;       // MQ4甲烷浓度 (ppm)
    float mq2_ppm;       // MQ2烟雾浓度 (ppm)
    float mq7_ppm;       // MQ7一氧化碳浓度 (ppm)
} SensorData_t;

/* 函数声明 */
void ADC_Driver_Init(void);
void ADC_ReadAllSensors(ADC_RawData_t *raw_data);
uint16_t ADC_ReadSingleChannel(SensorType_t sensor);
void ADC_ConvertToPPM(ADC_RawData_t *raw_data, SensorData_t *sensor_data);
float ADC_GetVoltage(uint16_t adc_value);

//
//
//滤波器声明


/* 全局滤波器实例（在adc_driver.c中定义） */
extern MovingAvgFilter_t mq4_ma_filter;
extern MovingAvgFilter_t mq2_ma_filter;
extern MovingAvgFilter_t mq7_ma_filter;

extern MedianFilter_t mq4_median_filter;
extern MedianFilter_t mq2_median_filter;
extern MedianFilter_t mq7_median_filter;

/* 新增函数：带滤波的读取 */
void ADC_ReadAllSensors_Filtered(ADC_RawData_t *raw_data);

#endif /* __ADC_DRIVER_H */
