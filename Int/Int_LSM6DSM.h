#ifndef __INT_LSM6DSM_H__
#define __INT_LSM6DSM_H__

#include "lsm6dsm_reg.h"
#include "i2c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "math.h"

typedef struct
{
    // x轴加速度
    int16_t acc_x;
    // y轴加速度
    int16_t acc_y;
    // z轴加速度
    int16_t acc_z;

    // x轴角速度
    int16_t gyro_x;
    // y轴角速度
    int16_t gyro_y;
    // z轴角速度
    int16_t gyro_z;
} AccGyroStruct;

// 存储角速度，加速度变量结构体
extern AccGyroStruct accgyro;

// 初始化配置
void Int_LSM6DSM_Init(void);

// 获取加速度和角速度
void Int_LSM6DSM_GetAccGyro(void);

#endif /* __INT_LSM6DSM_H__ */
