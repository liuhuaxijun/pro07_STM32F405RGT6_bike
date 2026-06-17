#include "Int_Power.h"

/**
 * @brief 初始化电源
 * 
 */
void Int_Power_Init(void)
{
    // 启动ADC
    HAL_ADC_Start(&hadc1);
}

/**
 * @brief 获取ADC测量的电池电压
 *
 * @return float
 */
float Int_Power_GetV(void)
{
    return 2 * HAL_ADC_GetValue(&hadc1) * 3.3 / 4095.0;
}

/**
 * @brief 电池是否充电中
 * 
 * @return true 
 * @return false 
 */
bool Int_Power_IsCharge(void)
{
    return HAL_GPIO_ReadPin(BAT_CHG_DET_GPIO_Port, BAT_CHG_DET_Pin) == GPIO_PIN_RESET;
}

/**
 * @brief 检查电源按键是否被按下
 *
 *
 * @return true
 * @return false
 */
bool Int_Power_PowerKeyIsPressed(void)
{
    if (HAL_GPIO_ReadPin(PWR_KEY_DEF_GPIO_Port, PWR_KEY_DEF_Pin) == GPIO_PIN_RESET)
    {
        // 延时消抖
        vTaskDelay(10);

        if (HAL_GPIO_ReadPin(PWR_KEY_DEF_GPIO_Port, PWR_KEY_DEF_Pin) == GPIO_PIN_RESET)
        {
            // 真实按下,等待用户松手
            while (1)
            {
                if (HAL_GPIO_ReadPin(PWR_KEY_DEF_GPIO_Port, PWR_KEY_DEF_Pin) == GPIO_PIN_SET)
                {
                    return true;
                }

                vTaskDelay(3);
            }
        }
    }
    return false;
}

/**
 * @brief 关闭电源
 *
 */
void Int_Power_PowerOFF(void)
{
    HAL_GPIO_WritePin(PWR_EN_GPIO_Port, PWR_EN_Pin, GPIO_PIN_RESET); 
}
