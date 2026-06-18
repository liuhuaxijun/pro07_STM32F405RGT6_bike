#ifndef __INT_ATGM336H_H__
#define __INT_ATGM336H_H__

#include "main.h"
#include "usart.h"
#include "string.h"
#include <stdio.h>

// gps接收缓冲区
#define MAXSIZE 1024
extern uint8_t gps_buffers[MAXSIZE];
extern uint16_t gps_sizes;

void Int_ATGM336H_Init(void);

#endif /* __INT_ATGM336H_H__ */
