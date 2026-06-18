#include "Int_ATGM336H.h"

// gps接收缓冲区
uint8_t gps_buffers[MAXSIZE] = "$GNGGA,070822.000,4006.81888,N,11621.89413,E,1,05,25.5,30.2,M,-9.6,M,,*58$GNGLL,4006.81888,N,11621.89413,E,070822.000,A,A*49$GNGSA,A,3,05,12,25,,,,,,,,,,25.5,25.5,25.5,1*02$GNGSA,A,3,10,13,,,,,,,,,,,25.5,25.5,25.5,4*05$GPGSV,3,1,11,05,54,263,24,06,32,098,,09,20,043,,11,68,061,,0*61$GPGSV,3,2,11,12,14,237,17,13,23,176,,15,05,204,,19,08,152,,0*60$GPGSV,3,3,11,20,75,327,,25,13,265,23,29,28,314,,0*51$BDGSV,2,1,05,08,64,301,,10,47,216,15,13,56,28$BDGSV,2,2,05,38,72,326,,0*48$GNRMC,070822.000,A,4006.81888,N,11621.89413,E,0.81,359.02,020624,,,A,V*02$GNVTG,359.02,T,,M,0.81,N,1.51,K,A*22$GNZDA,070822.000,02,06,2024,00,00*47$GPTXT,01,01,01,ANTENNA OPEN*253,10,27,59,133,,0*78";
// gps接收数据长度
uint16_t gps_sizes = 10;

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

