#include "Int_LSM6DSM.h"

// 用于存储六轴数据
AccGyroStruct accgyro;

// 设备地址
#define DEV_R 0xD5
#define DEV_W 0xD4

/**
 * @brief 写入寄存器
 *
 * @param handle
 * @param reg
 * @param buffer
 * @param len
 * @return int32_t
 */
int32_t write_reg(void *handle, uint8_t reg, const uint8_t *buffer, uint16_t len)
{

    HAL_I2C_Mem_Write((I2C_HandleTypeDef *)handle, DEV_W, reg, I2C_MEMADD_SIZE_8BIT, buffer, len, 1000);
};

/**
 * @brief 读取寄存器
 *
 * @param handle
 * @param reg
 * @param buf
 * @param len
 * @return int32_t
 */
int32_t read_reg(void *handle, uint8_t reg, uint8_t *buffer, uint16_t len)
{
    HAL_I2C_Mem_Read((I2C_HandleTypeDef *)handle, DEV_R, reg, I2C_MEMADD_SIZE_8BIT, buffer, len, 1000);
};

stmdev_ctx_t ctx = {
    // 写寄存器方式
    .write_reg = write_reg,
    // 读寄存器方式
    .read_reg = read_reg,
    // i2c句柄
    .handle = &hi2c1,
    // 延时函数
    .mdelay = vTaskDelay};

// 初始化配置
void Int_LSM6DSM_Init(void)
{
    // 1. 验证i2c
    uint8_t id = 0;
    lsm6dsm_device_id_get(&ctx, &id);
    if (id != LSM6DSM_ID)
    {
        return;
    }

    // 2. 复位
    lsm6dsm_reset_set(&ctx, 1);

    // 3。等待复位完成
    uint8_t is_run = 0;
    do
    {
        lsm6dsm_reset_get(&ctx, &is_run);
    } while (is_run != 0);

    // 4. 设置BDU 放置数据错位
    lsm6dsm_block_data_update_set(&ctx, 1);

    // 5. 设置加速度和角速度的量程
    lsm6dsm_gy_full_scale_set(&ctx, LSM6DSM_2000dps);   
    lsm6dsm_xl_full_scale_set(&ctx, LSM6DSM_8g);  

    // 6. 数据输出速率
    lsm6dsm_gy_data_rate_set(&ctx, LSM6DSM_GY_ODR_52Hz);
    lsm6dsm_xl_data_rate_set(&ctx, LSM6DSM_XL_ODR_52Hz);
}

// 获取加速度和角速度
void Int_LSM6DSM_GetAccGyro(void)
{
}
