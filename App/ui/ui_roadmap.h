#ifndef __UI_ROADMAP_H__
#define __UI_ROADMAP_H__

#include "ui_common.h"

// 创建地图页面。
void ui_roadmap_create(lv_obj_t *parent);

// 更新地图上的用户位置。
void ui_roadmap_update_location(float lat, float lon);

#endif
