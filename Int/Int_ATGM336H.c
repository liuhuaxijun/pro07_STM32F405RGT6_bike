#include "Int_ATGM336H.h"

// gps接收缓冲区
// uint8_t gps_buffers[MAXSIZE] = "$GNGGA,070822.000,4006.81888,N,11621.89413,E,1,05,25.5,30.2,M,-9.6,M,,*58$GNGLL,4006.81888,N,11621.89413,E,070822.000,A,A*49$GNGSA,A,3,05,12,25,,,,,,,,,,25.5,25.5,25.5,1*02$GNGSA,A,3,10,13,,,,,,,,,,,25.5,25.5,25.5,4*05$GPGSV,3,1,11,05,54,263,24,06,32,098,,09,20,043,,11,68,061,,0*61$GPGSV,3,2,11,12,14,237,17,13,23,176,,15,05,204,,19,08,152,,0*60$GPGSV,3,3,11,20,75,327,,25,13,265,23,29,28,314,,0*51$BDGSV,2,1,05,08,64,301,,10,47,216,15,13,56,28$BDGSV,2,2,05,38,72,326,,0*48$GNRMC,070822.000,A,4006.81888,N,11621.89413,E,0.81,359.02,020624,,,A,V*02$GNVTG,359.02,T,,M,0.81,N,1.51,K,A*22$GNZDA,070822.000,02,06,2024,00,00*47$GPTXT,01,01,01,ANTENNA OPEN*253,10,27,59,133,,0*78";
uint8_t gps_buffers[MAXSIZE] = "$GNGGA,070822.000,2237.55333,N,11350.27667,E,1,05,25.5,30.2,M,-9.6,M,,*5A$GNGLL,2237.55333,N,11350.27667,E,070822.000,A,A*4B$GNGSA,A,3,05,12,25,,,,,,,,,,25.5,25.5,25.5,1*02$GNGSA,A,3,10,13,,,,,,,,,,,25.5,25.5,25.5,4*05$GPGSV,3,1,11,05,54,263,24,06,32,098,,09,20,043,,11,68,061,,0*61$GPGSV,3,2,11,12,23,176,,15,05,204,,19,08,152,,0*60$GPGSV,3,3,11,20,75,327,,25,13,265,23,29,28,314,,0*51$BDGSV,2,1,05,08,64,301,,10,47,216,15,13,56,28$BDGSV,2,2,05,38,72,326,,0*48$GNRMC,070822.000,A,2237.55333,N,11350.27667,E,0.81,359.02,020624,,,A,V*00$GNVTG,359.02,T,,M,0.81,N,1.51,K,A*22$GNZDA,070822.000,02,06,2024,00,00*47$GPTXT,01,01,01,ANTENNA OPEN*253,10,27,59,133,,0*78";
// gps接收数据长度
uint16_t gps_sizes = 10;

// 常量定义(使用float 字面量)
static const float PI = 3.14159265358979324f;
// 克拉索夫斯基椭球体长半轴
static const float EE = 0.00669342162296594323f;
// 球体偏心率平方
static const float A = 6378245.0f;

// 发送命令
static void Int_ATGM336H_SendCMD(uint8_t *cmd);

void Int_ATGM336H_Init(void)
{
    Int_ATGM336H_SendCMD("PCAS01,1");
    Int_ATGM336H_SendCMD("PCAS04,2");
    Int_ATGM336H_SendCMD("PCAS02,1000");

    // HAL_StatusTypeDef receive_state = HAL_ERROR;
    // while (receive_state != HAL_OK)
    // {
    //     receive_state = HAL_UARTEx_ReceiveToIdle_IT(&huart2, gps_buffers, MAXSIZE);
    // }
}

static void Int_ATGM336H_SendCMD(uint8_t *cmd)
{
    uint8_t tmp_array[50] = {0};

    uint8_t check_sum = cmd[0];
    for (uint8_t i = 0; cmd[i] != '\0'; i++)
    {
        check_sum ^= cmd[i];
    }

    sprintf((char *)tmp_array, "$%s*%02X", cmd, check_sum);
    HAL_UART_Transmit(&huart2, tmp_array, strlen((char *)tmp_array), 1000);
}

void Int_ATGM336H_CallBack(uint16_t receive_sizes)
{
    gps_sizes = receive_sizes;
    // COM_DEBUG_LN("获取卫星的GPS数据:%s,收到数据的长度:%d", gps_buffers, gps_sizes);
    // 上一次GPS变长数据接收成功,立马接收下一次GPS卫星的数据!
    HAL_StatusTypeDef receive_state = HAL_ERROR;
    while (receive_state != HAL_OK)
    {
        receive_state = HAL_UARTEx_ReceiveToIdle_IT(&huart2, gps_buffers, MAXSIZE);
    }
}

/**
 * 判断坐标是否在中国境内
 * @param lat 纬度
 * @param lon 经度
 * @return true 表示在中国境外，不需要转换；false 表示在中国境内
 */
static bool out_of_china(float lat, float lon)
{
    if (lon < 72.004f || lon > 137.8347f)
        return true;
    if (lat < 0.8293f || lat > 55.8271f)
        return true;
    return false;
}

/**
 * 计算纬度偏移量
 */
static float transform_lat(float x, float y)
{
    float ret = -100.0f + 2.0f * x + 3.0f * y + 0.2f * y * y + 0.1f * x * y + 0.2f * sqrtf(fabsf(x));
    ret += (20.0f * sinf(6.0f * x * PI) + 20.0f * sinf(2.0f * x * PI)) * 2.0f / 3.0f;
    ret += (20.0f * sinf(y * PI) + 40.0f * sinf(y / 3.0f * PI)) * 2.0f / 3.0f;
    ret += (160.0f * sinf(y / 12.0f * PI) + 320.0f * sinf(y * PI / 30.0f)) * 2.0f / 3.0f;
    return ret;
}

/**
 * 计算经度偏移量
 */
static float transform_lon(float x, float y)
{
    float ret = 300.0f + x + 2.0f * y + 0.1f * x * x + 0.1f * x * y + 0.1f * sqrtf(fabsf(x));
    ret += (20.0f * sinf(6.0f * x * PI) + 20.0f * sinf(2.0f * x * PI)) * 2.0f / 3.0f;
    ret += (20.0f * sinf(x * PI) + 40.0f * sinf(x / 3.0f * PI)) * 2.0f / 3.0f;
    ret += (150.0f * sinf(x / 12.0f * PI) + 300.0f * sinf(x / 30.0f * PI)) * 2.0f / 3.0f;
    return ret;
}

/**
 * GPS坐标 (WGS-84) 转 GCJ-02 火星坐标
 * @param wgLat  输入的 WGS-84 纬度 (float)
 * @param wgLon  输入的 WGS-84 经度 (float)
 * @param mgLat  输出的 GCJ-02 纬度 (指针)
 * @param mgLon  输出的 GCJ-02 经度 (指针)
 */
void gps_to_gcj02(float wgLat, float wgLon, float *mgLat, float *mgLon)
{
    // 如果在中国境外，直接返回原坐标
    if (out_of_china(wgLat, wgLon))
    {
        *mgLat = wgLat;
        *mgLon = wgLon;
        return;
    }

    // 计算偏移量
    float dLat = transform_lat(wgLon - 105.0f, wgLat - 35.0f);
    float dLon = transform_lon(wgLon - 105.0f, wgLat - 35.0f);

    // 弧度转换
    float radLat = wgLat / 180.0f * PI;
    float magic = sinf(radLat);
    magic = 1.0f - EE * magic * magic;
    float sqrtMagic = sqrtf(magic);

    // 将偏移量从"度"转换为"经纬度差值"
    dLat = (dLat * 180.0f) / ((A * (1.0f - EE)) / (magic * sqrtMagic) * PI);
    dLon = (dLon * 180.0f) / (A / sqrtMagic * cosf(radLat) * PI);

    // 输出最终坐标
    *mgLat = wgLat + dLat;
    *mgLon = wgLon + dLon;
}
