#include "Int_CST6189.h"

#define DEV_ADDR_R 0x2B
#define DEV_ADDR_W 0x2A
#define CST816D_MAX_X 239U
#define CST816D_MAX_Y 319U

static bool is_pressed = false;

void Int_CST816_Callback(void)
{
    is_pressed = true;
}

void Int_CST816_ReadRegister(uint8_t reg, uint8_t *datas, uint8_t len)
{
    HAL_I2C_Mem_Read(&hi2c2, DEV_ADDR_R, reg, I2C_MEMADD_SIZE_8BIT, datas, len, 2000);
}

bool Int_CST816D_IsPressed(void)
{
    if (is_pressed)
    {
        is_pressed = false;
        return true;
    }

    return false;
}

void Int_CST816D_GetXY(uint16_t *x, uint16_t *y)
{
    uint8_t datas[4] = {0};
    uint16_t raw_x;
    uint16_t raw_y;

    Int_CST816_ReadRegister(0x03, datas, 4);

    raw_x = ((datas[0] & 0x0F) << 8) | datas[1];
    raw_y = ((datas[2] & 0x0F) << 8) | datas[3];

    if (raw_x >= CST816D_MAX_X)
    {
        *x = 0;
    }
    else
    {
        *x = CST816D_MAX_X - raw_x;
    }

    if (raw_y > CST816D_MAX_Y)
    {
        *y = CST816D_MAX_Y;
    }
    else
    {
        *y = raw_y;
    }
}
