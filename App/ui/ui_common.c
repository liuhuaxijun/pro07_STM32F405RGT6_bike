#include "ui_common.h"

// 定义常量
#ifndef M_PIT
#define M_PI 3.14159265358979323846f
#endif

// 卫星个数观察者
lv_subject_t saleNumSubject;
// 速度
lv_subject_t speedSubject;
// 骑行里程
lv_subject_t distanceSubject;
// 坡度
lv_subject_t slopeSubject;
// 经度
lv_subject_t lonSubject;
// 纬度
lv_subject_t latSubject;
// 骑行时间
lv_subject_t timeSubject;
// 缩放等级
lv_subject_t zoomSubject;
// 样式观察者
lv_subject_t styleSubject;

// 深色样式
lv_style_t darkStyle;
// 白色背景浅色样式
lv_style_t whiteBgStyle;
// 灰色背景浅色样式
lv_style_t greyBgStyle;

char * languages[] = {"English", "Chinese", NULL};
char * tags[] = {"Stopped", "Running", "Language", "DarkMode", "Roadmap", "General", "Settings", NULL};
char * translations[] = {
    "Stopped", "静止中 ",
    "Running", "运行中 ",
    "Language", "语言",
    "Dark Mode", "深色模式",
    "Roadmap", "路书",
    "General", "通用",
    "Settings", "设置"

};

/**
 * @brief 语言改变事件回调
 *
 * @param e 事件
 */
static void on_language_change(lv_event_t *e)
{
    lv_obj_t *label = lv_event_get_target_obj(e);
    const char *tag = (const char *)lv_event_get_user_data(e);

    lv_label_set_text(label, lv_tr(tag));
}

/**
 * @brief 给标签添加语言改变事件
 *
 * @param label 标签
 * @param tag 语言标签
 */
void ui_common_translation(lv_obj_t *label, char *tag)
{
    lv_label_set_text(label, lv_tr(tag));

    // 添加事件
    lv_obj_add_event_cb(label, on_language_change, LV_EVENT_TRANSLATION_LANGUAGE_CHANGED, tag);
}

/**
 * @brief 根据经纬度和缩放等级计算出所在的世界像素角标
 *
 * @param lng 经度
 * @param lat 纬度
 * @param zoom 缩放等级
 * @param x X 坐标
 * @param y Y 坐标
 */
void latlngToWorldPixelFloat(float lon, float lat, int zoom, int *x, int *y)
{
    // 1.计算当前缩放级别下的地图总像素边长(256*2^zoom)
    float mapSize = TITLE_SIZE * (float)(1 << zoom);

    // 2.经度转X像素
    *x = (int)((lon + 180.0f) / 360.0f * mapSize);

    // 3.纬度转 Y 像素
    // 限制纬度边界，防止 f1oat 溢出
    if (lat > 85.05112878f)
        lat = 85.05112878f;
    if (lat < -85.05112878f)
        lat = -85.05112878f;

    // 角度转弧度
    float latRad = lat * M_PI / 180.0f;

    // 使用单精度数学函数 sinf 和logf
    float sinLat = sinf(latRad);
    *y = (int)((0.5f - logf((1.0f + sinLat) / (1.0f - sinLat)) / (4.0f * M_PI)) * mapSize);
}

void convert_world_pixels(int64_t old_x, int64_t old_y, int old_zoom, int new_zoom, int64_t *new_x, int64_t *new_y)
{
    // 边界检查:指针不能为空，缩放等级通常在0到 30之间(防止位移溢出)
    if (!new_x || !new_y || old_zoom < 0 || new_zoom < 0 || old_zoom > 31 || new_zoom > 31)
    {
        return ;
    }

    if (new_zoom >= old_zoom)
    {
        // 放大(ZoomIn):坐标变大，向左位移
        int delta = new_zoom - old_zoom;
        *new_x = old_x << delta;
        *new_y = old_y << delta;
    }
    else
    {
        //缩小(Zoom Out):坐标变小，向右位移
        int delta = old_zoom - new_zoom;
        *new_x = old_x >> delta;
        *new_y = old_y >> delta;
    }
}
