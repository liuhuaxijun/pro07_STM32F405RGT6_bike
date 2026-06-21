#ifndef __UI_H__
#define __UI_H__

#include "ui_common.h"
#include "ui_settings.h"
#include "ui_general.h"
#include "ui_roadmap.h"

void ui_create(void);

/**
 * @brief 更新电池电量
 * 
 * @param isCharge 
 * @param v 
 */
void ui_updateBattery(bool isCharge, float v);

/**
 * @brief 更新运动状态
 *
 * @param speed
 */
void ui_updateBikeState(float speed);

/**
 * @brief 更新位置
 * 
 * @param lat 
 * @param lon 
 */
void ui_updateLocation(float lat, float lon);

#endif
