#ifndef __INT_ATGM336H_H__
#define __INT_ATGM336H_H__

#include "main.h"
#include "usart.h"
#include "string.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>


// gps接收缓冲区
#define MAXSIZE 1024
extern uint8_t gps_buffers[MAXSIZE];
extern uint16_t gps_sizes;

// 初始化
void Int_ATGM336H_Init(void);

// 转换为国测坐标系
void gps_to_gcj02(float wgLat, float wgLon, float *mgLat, float *mgLon);

#endif /* __INT_ATGM336H_H__ */
