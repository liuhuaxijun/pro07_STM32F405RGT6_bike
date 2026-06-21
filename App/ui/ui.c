#include "ui.h"

// mmm : ss\0
// 用于存储骑行时间的缓冲
char timeBuff[9];
// 用于存储上一次骑行时间的缓冲
char beforetimeBuff[9];

// 电池电量标签
lv_obj_t *batteryLabel = NULL;
// 运动状态标签
lv_obj_t *runStateLabel = NULL;

/**
 * @brief 初始化样式
 *
 *
 */
static void ui_style_init(void)
{
    // 初始化样式
    lv_style_init(&darkStyle);
    // 黑色背景
    lv_style_set_bg_color(&darkStyle, lv_color_black());
    // 白色文本
    lv_style_set_text_color(&darkStyle, lv_color_white());
    // 去掉内边距
    lv_style_set_pad_all(&darkStyle, 0);
    // 去掉边框
    lv_style_set_border_width(&darkStyle, 0);
    // 去掉圆角
    lv_style_set_radius(&darkStyle, 0);

    lv_style_init(&whiteBgStyle);
    // 白色背景
    lv_style_set_bg_color(&whiteBgStyle, lv_color_white());
    // 黑色文本
    lv_style_set_text_color(&whiteBgStyle, lv_color_black());
    // 去掉内边距
    lv_style_set_pad_all(&whiteBgStyle, 0);
    // 去掉边框
    lv_style_set_border_width(&whiteBgStyle, 0);
    // 去掉圆角
    lv_style_set_radius(&whiteBgStyle, 0);

    lv_style_init(&greyBgStyle);
    // 灰色背景
    lv_style_set_bg_color(&greyBgStyle, lv_palette_main(LV_PALETTE_GREY));
    // 黑色文本
    lv_style_set_text_color(&greyBgStyle, lv_color_black());
    // 去掉内边距
    lv_style_set_pad_all(&greyBgStyle, 0);
    // 去掉边框
    lv_style_set_border_width(&greyBgStyle, 0);
    // 去掉圆角
    lv_style_set_radius(&greyBgStyle, 0);
}

// 初始化观察者
static void ui_subject_init(void)
{
    // 初始化卫星个数观察者
    lv_subject_init_int(&saleNumSubject, 1);
    // 初始化速度观察者
    lv_subject_init_float(&speedSubject, 1.2f);
    // 初始化骑行里程观察者
    lv_subject_init_float(&distanceSubject, 0.2f);
    // 初始化坡度观察者
    lv_subject_init_float(&slopeSubject, 0.0f);
    // 初始化经度观察者
    lv_subject_init_float(&lonSubject, 113.835534f);
    // 初始化纬度观察者
    lv_subject_init_float(&latSubject, 22.628743f);
    // 初始化骑行时间观察者
    lv_subject_init_string(&timeSubject, timeBuff, beforetimeBuff, 9, "00 : 00");
    // 初始化缩放等级观察者
    lv_subject_init_int(&zoomSubject, 14);
    // 初始化样式观察者
    lv_subject_init_int(&styleSubject, 0);
}

/**
 * @brief 创建标题
 *
 * @param title 标题对象
 */
static void ui_create_title(lv_obj_t *title)
{
    // 创建卫星个数
    lv_obj_t *saleLabel = lv_label_create(title);

    lv_label_bind_text(saleLabel, &saleNumSubject, LV_SYMBOL_GPS " %d");
    lv_obj_align(saleLabel, LV_ALIGN_LEFT_MID, 10, 0);

    // 运动状态
    runStateLabel = lv_label_create(title);
    // lv_label_set_text(runStateLabel, "Stopped");
    ui_common_translation(runStateLabel, tags[0]);
    lv_obj_align(runStateLabel, LV_ALIGN_CENTER, 0, 0);

    // 电池电量
    batteryLabel = lv_label_create(title);
    lv_label_set_text(batteryLabel, LV_SYMBOL_BATTERY_FULL);
    lv_obj_align(batteryLabel, LV_ALIGN_RIGHT_MID, -10, 0);
}

/**
 * @brief 创建内容
 *
 * @param content 内容对象
 */
static void ui_create_content(lv_obj_t *content)
{
    // 创建标签视图实现页面切换
    lv_obj_t *tabview = lv_tabview_create(content);
    lv_obj_bind_style(tabview, &darkStyle, 0, &styleSubject, 1);
    lv_obj_bind_style(tabview, &whiteBgStyle, 0, &styleSubject, 0);

    // 添加标签栏
    lv_obj_t *Roadmap = lv_tabview_add_tab(tabview, "Roadmap");
    lv_obj_t *General = lv_tabview_add_tab(tabview, "General");
    lv_obj_t *Settings = lv_tabview_add_tab(tabview, "Settings");

    // 设置标签栏显示位置
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_BOTTOM);

    // 设置标签栏的大小
    lv_tabview_set_tab_bar_size(tabview, 25);

    // 设置标签栏的背景透明度为不透明
    lv_obj_set_style_bg_opa(Roadmap, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_opa(General, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_opa(Settings, LV_OPA_COVER, 0);

    // 获取标签栏容器
    lv_obj_t *tabBar = lv_tabview_get_tab_bar(tabview);

    // 设置tab bar里面的标签的翻译
    ui_common_translation(lv_obj_get_child(lv_obj_get_child(tabBar, 0), 0), tags[4]);
    ui_common_translation(lv_obj_get_child(lv_obj_get_child(tabBar, 1), 0), tags[5]);
    ui_common_translation(lv_obj_get_child(lv_obj_get_child(tabBar, 2), 0), tags[6]);

    // 获取tabview内容容器
    lv_obj_t *tabContent = lv_tabview_get_content(tabview);
    lv_obj_bind_style(tabBar, &darkStyle, 0, &styleSubject, 1);
    lv_obj_bind_style(tabBar, &whiteBgStyle, 0, &styleSubject, 0);

    // 去除滚动翻页
    lv_obj_remove_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);

    // 创建设置页面
    ui_settings_create(Settings);

    // 创建骑行显示界面
    ui_general_create(General);

    // 创建路书界面
    ui_roadmap_create(Roadmap);

    // 设置透明度为不透明
    // // 设置背景透明度
    // lv_obj_set_style_bg_opa(tab1, LV_OPA_COVER, 0);
    // lv_obj_set_style_bg_opa(tab2, LV_OPA_COVER, 0);
    // lv_obj_set_style_bg_opa(tab3, LV_OPA_COVER, 0);
    // // 设置标签栏的背景颜色
    // lv_obj_set_style_bg_color (tab1, lv_palette_main(LV_PALETTE_BLUE),0);
    // lv_obj_set_style_bg_color (tab2, lv_palette_main(LV_PALETTE_RED),0);
    // lv_obj_set_style_bg_color (tab3, lv_palette_main(LV_PALETTE_GREEN),0);
}

// 画页面
static void ui_create_page(void)
{
    // 1. 获取活动屏幕
    lv_obj_t *scr = lv_scr_act();

    // 2. 创建页面容器
    lv_obj_t *page = lv_obj_create(scr);
    // 绑定样式
    lv_obj_bind_style(page, &darkStyle, 0, &styleSubject, 1);
    lv_obj_bind_style(page, &whiteBgStyle, 0, &styleSubject, 0);
    lv_obj_set_size(page, lv_pct(100), lv_pct(100));

    // 3、弹性布局
    // 3.1、声明布局方式
    lv_obj_set_layout(page, LV_LAYOUT_FLEX);
    // 3.2 指定布局方向
    lv_obj_set_flex_flow(page, LV_FLEX_FLOW_COLUMN);
    // 3.3 去掉间隙
    lv_obj_set_style_pad_row(page, 0, 0);
    lv_obj_set_style_pad_column(page, 0, 0);
    // 3.4 添加子元素
    // 标题
    lv_obj_t *title = lv_obj_create(page);
    lv_obj_set_width(title, lv_pct(100));
    // 绑定样式
    lv_obj_bind_style(title, &darkStyle, 0, &styleSubject, 1);
    lv_obj_bind_style(title, &greyBgStyle, 0, &styleSubject, 0);
    // lv_obj_set_style_bg_color(title, lv_palette_main(LV_PALETTE_BLUE), 0);
    // 设置占比
    lv_obj_set_flex_grow(title, 1);
    ui_create_title(title);

    // 内容
    lv_obj_t *content = lv_obj_create(page);
    lv_obj_set_width(content, lv_pct(100));
    lv_obj_bind_style(content, &darkStyle, 0, &styleSubject, 1);
    lv_obj_bind_style(content, &whiteBgStyle, 0, &styleSubject, 0);
    // lv_obj_set_style_bg_color(content, lv_palette_main(LV_PALETTE_RED), 0);
    // 设置占比
    lv_obj_set_flex_grow(content, 9);
    ui_create_content(content);
}

void ui_create(void)
{
    // 初始化样式
    ui_style_init();

    // 初始化观察者
    ui_subject_init();

    // 添加静态翻译
    lv_translation_add_static((const char **)languages, (const char **)tags, (const char **)translations);
    lv_translation_set_language(languages[0]);  // English

    // 创建UI页面
    ui_create_page();
}

void ui_updateBattery(bool isCharge, float v)
{
    if (isCharge)
    {
        lv_label_set_text(batteryLabel, LV_SYMBOL_CHARGE);
    }
    else
    {
        // 显示电池电量
        if (v > 4.1f)
        {
            lv_label_set_text(batteryLabel, LV_SYMBOL_BATTERY_FULL);
        }
        else if (v > 4.0f)
        {
            lv_label_set_text(batteryLabel, LV_SYMBOL_BATTERY_3);
        }
        else if (v > 3.8f)
        {
            lv_label_set_text(batteryLabel, LV_SYMBOL_BATTERY_2);
        }
        else if (v > 3.6f)
        {
            lv_label_set_text(batteryLabel, LV_SYMBOL_BATTERY_1);
        }
        else
        {
            lv_label_set_text(batteryLabel, LV_SYMBOL_BATTERY_EMPTY);
        }
    }
}

/**
 * @brief 更新运动状态
 *
 * @param speed
 */
void ui_updateBikeState(float speed)
{
    if (speed > 0.5f)
    {
        lv_label_set_text(runStateLabel, lv_tr(tags[1]));
    }
    else
    {
        lv_label_set_text(runStateLabel, lv_tr(tags[0]));
    }
}

/**
 * @brief 更新位置信息
 * 
 * @param lat 
 * @param lon 
 */
void ui_updateLocation(float lat, float lon)
{
    
}
