#ifndef __INT_CST6189_H__
#define __INT_CST6189_H__

#include "i2c.h"
#include "stdbool.h"

/**
 * @brief 用户是否按下手指
 * 
 * @return true 
 * @return false 
 */
bool Int_CST816D_IsPressed(void);

/**
 * @brief 用户按下手指的角标
 * 
 * @param x 
 * @param y 
 */
void Int_CST816D_GetXY(uint16_t *x, uint16_t *y);


#endif /* __INT_CST6189_H__ */
