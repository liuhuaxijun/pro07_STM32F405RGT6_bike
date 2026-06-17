#ifndef __INT_POWER_H__
#define __INT_POWER_H__

#include "gpio.h"
#include "stdbool.h"
#include "FreeRTOS.h"
#include "task.h"
#include "adc.h"

/**
 * @brief 初始化电源
 *
 */
void Int_Power_Init(void);

/**
 * @brief 检查电源按键是否被按下
 *
 *
 * @return true
 * @return false
 */
bool Int_Power_PowerKeyIsPressed(void);

/**
 * @brief 关闭电源
 *
 */
void Int_Power_PowerOFF(void);

/**
 * @brief 获取ADC测量的电池电压
 *
 * @return float
 */
float Int_Power_GetV(void);

/**
 * @brief 电池是否充电中
 * 
 * @return true 
 * @return false 
 */
bool Int_Power_IsCharge(void);

#endif /* __INT_POWER_H__ */
