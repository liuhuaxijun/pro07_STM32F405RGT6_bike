#ifndef __UI_H__
#define __UI_H__

#include "ui_common.h"
#include "ui_settings.h"
#include "ui_general.h"
#include "ui_roadmap.h"

void ui_create(void);

// 更新电池电量
void ui_updateBattery(bool isCharge, float v);

/**
 * @brief 更新运动状态
 *
 * @param speed
 */
void ui_updateBikeState(float speed);

#endif
