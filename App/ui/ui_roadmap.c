#include "ui_roadmap.h"
#include "Com_Debug.h"
#include "ff.h"

typedef struct
{
    // 用户所在世界的像素坐标X
    int world_x;
    // 用户所在世界的像素坐标Y
    int world_y;
    // 用户经纬度所在图片的x角标
    int title_x;
    // 用户经纬度所在图片的y角标
    int title_y;
    // 用户经纬度所在图片相对世界像素角标
    int relav_world_x;
    // 用户经纬度所在图片相对世界像素角标
    int relav_world_y;

    // 屏幕中心点所在世界的像素坐标X
    int center_world_x;
    // 屏幕中心点所在世界的像素坐标Y
    int center_world_y;
    // 屏幕中心点经纬度所在图片的x角标
    int center_title_x;
    // 屏幕中心点经纬度所在图片的y角标
    int center_title_y;
    // 屏幕中心点经纬度所在图片相对世界像素角标
    int center_relav_world_x;
    // 屏幕中心点经纬度所在图片相对世界像素角标
    int center_relav_world_y;
} TitleStruct;

TitleStruct title;

// 九宫格图片
lv_obj_t *imgs[9];
// 图片路径
char imgPath[50];

// 地图屏幕宽高
uint32_t scr_w = 0, scr_h = 0;

// 上一次滚动条的角标
int32_t last_scroll_x = 0, last_scroll_y = 0;
// 是否用户滚动
bool is_user_scroll = true;

// 用户红点
lv_obj_t *userPoint = NULL;

// 旧的缩放等级
int last_zoom = 0;

/**
 *  @brief 加载地图
 *  @param div 九宫格容器
 *  @param parent 父容器
 * */
static void load_map(lv_obj_t *div, lv_obj_t *parent)
{
    for (uint8_t i = 0; i < 9; i++)
    {
        if (imgs[i] == NULL)
        {
            // 创建图片组件
            imgs[i] = lv_img_create(div);
            // 将子组件添加到网格
            lv_obj_set_grid_cell(imgs[i], LV_GRID_ALIGN_STRETCH, i % 3, 1, LV_GRID_ALIGN_STRETCH, i / 3, 1);
        }

        // 加载图片
        // x + i % 3 - 1
        // y + i / 3 - 1
        // S:/z/x/y/tile.bmp
        sprintf(imgPath, "S:/%d/%d/%d/tile.bmp", lv_subject_get_int(&zoomSubject), title.center_title_x + i % 3 - 1, title.center_title_y + i / 3 - 1);
        COM_DEBUG_LN("map tile: %s", imgPath);
        {
            char fatfs_path[50];
            FILINFO file_info;
            FRESULT stat_res;

            sprintf(fatfs_path, "%smap/%d/%d/%d/tile.bmp", "0:/", lv_subject_get_int(&zoomSubject), title.center_title_x + i % 3 - 1, title.center_title_y + i / 3 - 1);
            stat_res = f_stat(fatfs_path, &file_info);
            COM_DEBUG_LN("f_stat(%s) = %d", fatfs_path, stat_res);
        }
        lv_img_set_src(imgs[i], imgPath);
    }

    // 滚到到用户位置作为屏幕中心点
    last_scroll_x = TITLE_SIZE + title.center_relav_world_x - scr_w / 2;
    last_scroll_y = TITLE_SIZE + title.center_relav_world_y - scr_h / 2;
    is_user_scroll = false;
    lv_obj_scroll_to(parent, last_scroll_x, last_scroll_y, LV_ANIM_OFF);
    is_user_scroll = true;

    if (userPoint)
    {
        // 计算出九宫格左上角的世界像素角标
        int32_t x = title.center_world_x - title.center_relav_world_x - TITLE_SIZE;
        int32_t y = title.center_world_y - title.center_relav_world_y - TITLE_SIZE;

        // 计算出用户红点相对于容器左上角的位置
        int32_t pos_x = title.world_x - x;
        int32_t pos_y = title.world_y - y;

        // 设置红点的位置
        lv_obj_set_pos(userPoint, pos_x - 15, pos_y - 15);

        // 忽略布局影响
        lv_obj_add_flag(userPoint, LV_OBJ_FLAG_IGNORE_LAYOUT);
        // 移动到最顶层
        lv_obj_move_foreground(userPoint);
    }
}

/**
 *  @brief 回到原点按钮点击事件
 * */
void return_loc_cb(lv_event_t *e)
{
    // 只要点击回到原点的图标，不管之前那的缩放等级是多少，都应该变为最细的缩放等级
    lv_subject_set_int(&zoomSubject, 14);
    last_zoom = 14;

    // 重新计算缩放等级为14的角标
    latlngToWorldPixelFloat(lv_subject_get_float(&lonSubject), lv_subject_get_float(&latSubject), lv_subject_get_int(&zoomSubject), &title.world_x, &title.world_y);
    title.relav_world_x = title.world_x % TITLE_SIZE;
    title.relav_world_y = title.world_y % TITLE_SIZE;
    title.title_x = title.world_x / TITLE_SIZE;
    title.title_y = title.world_y / TITLE_SIZE;

    // printf("-------------------------\n");
    // 把屏幕中心点的世界像素坐标设置为用户当前位置的世界像素坐标
    title.center_world_x = title.world_x;
    title.center_world_y = title.world_y;
    // 用户当前位置在当前瓦片内部的相对像素坐标
    title.center_relav_world_x = title.relav_world_x;
    title.center_relav_world_y = title.relav_world_y;
    // 用户当前位置在当前瓦片内部的相对像素坐标
    title.center_title_x = title.title_x;
    title.center_title_y = title.title_y;

    lv_obj_t *mapDiv = (lv_obj_t *)lv_event_get_user_data(e);

    // 刷图
    load_map(lv_obj_get_child(mapDiv, 0), mapDiv);
}

/**
 *  @brief 缩放等级滑动条事件
 */
void zoom_cb(lv_event_t *e)
{
    // printf("zoom = %d\n", lv_subject_get_int(&zoomSubject));
    // 根据旧的缩放等级世界像素角标，计算出新的缩放等级的世界像素角标
    convert_world_pixels(title.center_world_x, title.center_world_y, last_zoom, lv_subject_get_int(&zoomSubject), (int64_t *)&title.center_world_x, (int64_t *)&title.center_world_y);

    // 更新屏幕其他角标
    title.center_title_x = title.center_world_x / TITLE_SIZE;
    title.center_title_y = title.center_world_y / TITLE_SIZE;
    title.center_relav_world_x = title.center_world_x % TITLE_SIZE;
    title.center_relav_world_y = title.center_world_y % TITLE_SIZE;

    // 重新计算用户经纬度在当前缩放等级所在的相关角标
    latlngToWorldPixelFloat(lv_subject_get_float(&lonSubject), lv_subject_get_float(&latSubject), lv_subject_get_int(&zoomSubject), &title.world_x, &title.world_y);
    title.relav_world_x = title.world_x % TITLE_SIZE;
    title.relav_world_y = title.world_y % TITLE_SIZE;
    title.title_x = title.world_x / TITLE_SIZE;
    title.title_y = title.world_y / TITLE_SIZE;

    last_zoom = lv_subject_get_int(&zoomSubject);
    // 获取传入的参数
    lv_obj_t *mapDiv = (lv_obj_t *)lv_event_get_user_data(e);

    // 刷图
    load_map(lv_obj_get_child(mapDiv, 0), mapDiv);
}

/**
 *  @brief 地图屏幕窗口滚动事件
 */
void map_scroll_cb(lv_event_t *e)
{
    // 如果是程序滚动，不需要计算
    if (!is_user_scroll)
        return;

    // 获取目标组件
    lv_obj_t *obj = lv_event_get_target_obj(e);
    // 获取当前滚动条的角标
    int32_t x = lv_obj_get_scroll_x(obj);
    int32_t y = lv_obj_get_scroll_y(obj);

    // 计算滚动的距离
    int32_t scroll_x = x - last_scroll_x;
    int32_t scroll_y = y - last_scroll_y;

    // 计算出当前屏幕中心点对应的世界像素角标
    title.center_world_x += scroll_x;
    title.center_world_y += scroll_y;

    // 计算出当前屏幕中心点所在的图片的角标
    int tile_x = title.center_world_x / TITLE_SIZE;
    int tile_y = title.center_world_y / TITLE_SIZE;

    // 更新上一次的滚动位置
    last_scroll_x = x;
    last_scroll_y = y;
    // 还在同一张瓦片里
    if (tile_x == title.center_title_x && tile_y == title.center_title_y)
    {
        // 屏幕中心还在中心图片
        return;
    }

    // 屏幕中心点移出了中心图片，需要重新加载新的九宫格地图

    // 更新屏幕中心点的角标
    title.center_title_x = tile_x;
    title.center_title_y = tile_y;
    title.center_relav_world_x = title.center_world_x % TITLE_SIZE;
    title.center_relav_world_y = title.center_world_y % TITLE_SIZE;

    // 刷图
    load_map(lv_obj_get_child(obj, 0), obj);

    // printf("center_title_x: %d, center_title_y: %d\n", title.center_title_x, title.center_title_y);
    // printf("tile_x: %d, tile_y: %d\n", tile_x, tile_y);
}

/**
 * @brief 处理地图逻辑
 *
 * @param parent 地图容器
 */
static void ui_roadMap_processMap(lv_obj_t *parent)
{
    // 0. 创建九宫格网格
    lv_obj_t *div = lv_obj_create(parent);
    lv_obj_set_size(div, TITLE_SIZE * 3, TITLE_SIZE * 3);
    lv_obj_add_style(div, &whiteBgStyle, 0);
    lv_obj_set_layout(div, LV_LAYOUT_GRID);

    // 声明网格
    static int32_t column_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(div, column_dsc, row_dsc);

    // 去掉间隙
    lv_obj_set_style_pad_row(div, 0, 0);
    lv_obj_set_style_pad_column(div, 0, 0);

    // 1. 默认九宫格中心图片是用户经纬度所在的图片，根据用户经纬度计算出世界像素角标
    latlngToWorldPixelFloat(lv_subject_get_float(&lonSubject), lv_subject_get_float(&latSubject), lv_subject_get_int(&zoomSubject), &title.world_x, &title.world_y);

    // 2. 计算出图片角标
    title.title_x = title.world_x / TITLE_SIZE;
    title.title_y = title.world_y / TITLE_SIZE;

    // 计算出用户经纬度所在图片相对世界像素角标
    title.relav_world_x = title.world_x % TITLE_SIZE;
    title.relav_world_y = title.world_y % TITLE_SIZE;

    // 默认用户经纬度所在位置就是屏幕中心
    title.center_world_x = title.world_x;
    title.center_world_y = title.world_y;
    title.center_title_x = title.title_x;
    title.center_title_y = title.title_y;
    title.center_relav_world_x = title.relav_world_x;
    title.center_relav_world_y = title.relav_world_y;

    // 创建用户红点
    userPoint = lv_obj_create(div);
    lv_obj_set_style_bg_color(userPoint, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_size(userPoint, 30, 30);
    // 设置圆角
    lv_obj_set_style_radius(userPoint, LV_RADIUS_CIRCLE, 0);
    // lv_obj_set_pos(userPoint, 400 ,400);

    // 3. 向九宫格容器添加图片
    load_map(div, parent);
}

void ui_roadmap_create(lv_obj_t *parent)
{
    // 绑定样式
    lv_obj_bind_style(parent, &darkStyle, 0, &styleSubject, 1);
    lv_obj_bind_style(parent, &whiteBgStyle, 0, &styleSubject, 0);

    // 弹性布局
    // 1. 声明布局方式
    lv_obj_set_layout(parent, LV_LAYOUT_FLEX);

    // 2. 声明排列方向
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);

    // 3.去除间隙
    lv_obj_set_style_pad_row(parent, 0, 0);
    lv_obj_set_style_pad_column(parent, 0, 0);

    // 4. 添加子组件
    lv_obj_t *mapDiv = lv_obj_create(parent);
    lv_obj_set_width(mapDiv, lv_pct(100));
    lv_obj_set_style_pad_all(mapDiv, 0, 0);
    lv_obj_set_style_border_width(mapDiv, 0, 0);
    lv_obj_set_flex_grow(mapDiv, 9);
    lv_obj_set_style_radius(mapDiv, 0, 0);
    lv_obj_set_style_bg_color(mapDiv, lv_palette_main(LV_PALETTE_BLUE), 0);

    // 取消滚动条的显示
    lv_obj_set_scrollbar_mode(mapDiv, LV_SCROLLBAR_MODE_OFF);

    // 添加地图图片
    // lv_obj_t *Img = lv_image_create(mapDiv);
    // lv_img_set_src(Img, "S:/14/13388/7126/tile.bmp");

    // 获取地图窗口的宽高
    lv_obj_update_layout(mapDiv);
    scr_w = lv_obj_get_width(mapDiv);
    scr_h = lv_obj_get_height(mapDiv);
    // printf("scr_w: %d, scr_h: %d\n", scr_w, scr_h);

    // 关闭弹性动量
    lv_obj_remove_flag(mapDiv, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    // 关闭弹性滚动
    lv_obj_remove_flag(mapDiv, LV_OBJ_FLAG_SCROLL_ELASTIC);

    // 添加滚动事件
    lv_obj_add_event_cb(mapDiv, map_scroll_cb, LV_EVENT_SCROLL_END, NULL);

    // 5. 加载地图
    ui_roadMap_processMap(mapDiv);

    // 创建缩放等级滑动条
    lv_obj_t *zoomSlider = lv_slider_create(parent);
    lv_obj_set_size(zoomSlider, 10, 150);
    // 让 zoomSlider 重新参与父容器的自动布局计算
    lv_obj_add_flag(zoomSlider, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_align(zoomSlider, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_move_foreground(zoomSlider);
    // 设置缩放等级滑动条的范围
    lv_slider_set_range(zoomSlider, 3, 14);
    lv_slider_bind_value(zoomSlider, &zoomSubject);
    last_zoom = lv_subject_get_int(&zoomSubject);
    lv_obj_add_event_cb(zoomSlider, zoom_cb, LV_EVENT_VALUE_CHANGED, mapDiv);

    // 创建回到原点按钮
    lv_obj_t *pointBt = lv_button_create(parent);
    lv_obj_t *pointLabel = lv_label_create(pointBt);
    lv_label_set_text(pointLabel, LV_SYMBOL_TINT);
    lv_obj_set_size(pointBt, 20, 20);
    lv_obj_align(pointBt, LV_ALIGN_BOTTOM_LEFT, 6, -35);
    // 忽略布局的影响
    lv_obj_add_flag(pointBt, LV_OBJ_FLAG_IGNORE_LAYOUT);
    // 移动到最顶层
    lv_obj_move_foreground(pointBt);
    // 文字对齐
    lv_obj_align(pointLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_align(pointLabel, LV_TEXT_ALIGN_CENTER, 0);

    // 添加点击事件
    lv_obj_add_event_cb(pointBt, return_loc_cb, LV_EVENT_CLICKED, mapDiv);

    lv_obj_t *infoDiv = lv_obj_create(parent);
    lv_obj_set_width(infoDiv, lv_pct(100));
    lv_obj_bind_style(infoDiv, &darkStyle, 0, &styleSubject, 1);
    lv_obj_bind_style(infoDiv, &greyBgStyle, 0, &styleSubject, 0);
    lv_obj_set_flex_grow(infoDiv, 1);
    // lv_obj_set_style_bg_color(infoDiv, lv_palette_main(LV_PALETTE_RED), 0);

    // 显示速度和里程
    lv_obj_t *speedLabel = lv_label_create(infoDiv);
    lv_label_bind_text(speedLabel, &speedSubject, "%.1f km/h");
    lv_obj_align(speedLabel, LV_ALIGN_LEFT_MID, 15, 0);

    lv_obj_t *distanceLabel = lv_label_create(infoDiv);
    lv_label_bind_text(distanceLabel, &distanceSubject, "%.2f km");
    lv_obj_align(distanceLabel, LV_ALIGN_RIGHT_MID, -15, 0);
}
