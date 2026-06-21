/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Com_Debug.h"
#include "Int_CST6189.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
// #include "demos/lv_demos.h"
#include "ui.h"
#include "fatfs.h"
#include "Int_Power.h"
#include "Int_ATGM336H.h"
#include "Int_LSM6DSM.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
bool charge = false;
float v = 0.0f;

/**
 * @brief 更新lvgl的电池电量图标
 *
 * @param args
 */
void update_battery_callback(void *args)
{
  ui_updateBattery(charge, v);
}

// 卫星个数
uint8_t saleNum = 0;
// 存储经纬度数据信息
float lon = 0.0f, lat = 0.0f;
char lonDir = 0, latDir = 0;
// 存储速度信息
float speed = 0.0f, distance = 0.0f;
// 骑行时间
int time = 0;

/**
 * @brief 更新lvgl的骑行数据
 *
 * @param args
 */
void update_bike_cb(void *args)
{
  // 更新卫星个数
  lv_subject_set_int(&saleNumSubject, saleNum);

  // 更新骑行速度
  lv_subject_set_float(&speedSubject, speed);

  // 更新骑行距离
  lv_subject_set_float(&distanceSubject, distance);

  // 更新经纬度
  lv_subject_set_float(&lonSubject, lon);
  lv_subject_set_float(&latSubject, lat);

  // 骑行时间
  char timestr[9] = {0};
  sprintf(timestr, "%d : %d", time / 60, time % 60);
  lv_subject_copy_string(&timeSubject, timestr);

  // 更新运动状态
  ui_updateBikeState(speed);

  // 根据经纬度更新用户地图中的位置【考试】
  ui_updateLocation(lat, lon);
}

// 坡度百分比
float slope_percentage = 0.0f;
/**
 * @brief 更新坡度
 * 
 * @param args 
 */
void update_slope(void *args)
{ 
  // 把当前计算出来的坡度值 slope_percentage 写入 LVGL 的 subject 数据源 slopeSubject
  lv_subject_set_float(&slopeSubject, slope_percentage);
}

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for mainTask */
osThreadId_t mainTaskHandle;
const osThreadAttr_t mainTask_attributes = {
    .name = "mainTask",
    .stack_size = 4096 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for powerTask */
osThreadId_t powerTaskHandle;
const osThreadAttr_t powerTask_attributes = {
    .name = "powerTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for gpsTask */
osThreadId_t gpsTaskHandle;
const osThreadAttr_t gpsTask_attributes = {
    .name = "gpsTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityBelowNormal,
};
/* Definitions for slopeTask */
osThreadId_t slopeTaskHandle;
const osThreadAttr_t slopeTask_attributes = {
    .name = "slopeTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityLow7,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void MainTaskFunc(void *argument);
void PowerTaskFunc(void *argument);
void GPSTaskFunc(void *argument);
void slopeTaskFunc(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of mainTask */
  mainTaskHandle = osThreadNew(MainTaskFunc, NULL, &mainTask_attributes);

  /* creation of powerTask */
  powerTaskHandle = osThreadNew(PowerTaskFunc, NULL, &powerTask_attributes);

  /* creation of gpsTask */
  gpsTaskHandle = osThreadNew(GPSTaskFunc, NULL, &gpsTask_attributes);

  /* creation of slopeTask */
  slopeTaskHandle = osThreadNew(slopeTaskFunc, NULL, &slopeTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_MainTaskFunc */
/**
 * @brief  Function implementing the mainTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_MainTaskFunc */
void MainTaskFunc(void *argument)
{
  /* USER CODE BEGIN MainTaskFunc */
  /* Infinite loop */

  // 挂载文件系统
  FRESULT mount_res = f_mount(&SDFatFS, SDPath, 1);
  COM_DEBUG_LN("f_mount(%s) = %d", SDPath, mount_res);

  // 1. 初始化LVGL
  lv_init();

  // 设置获取当前时间回调
  lv_tick_set_cb(xTaskGetTickCount);
  // 设置延时回调
  lv_delay_set_cb(vTaskDelay);

  // 2. 初始化输入，显示设备
  lv_port_disp_init();
  lv_port_indev_init();

  // 3. 创建UI
  // lv_demo_widgets();
  ui_create();

  // uint16_t x = 0, y = 0;
  for (;;)
  {
    // if (Int_CST816D_IsPressed())
    // {
    //   Int_CST816D_GetXY(&x, &y);
    //   COM_DEBUG_LN("x == %d, y == %d", x, y);
    // }

    // 下一次循环等待的时间
    uint32_t tm = lv_timer_handler();
    if (tm < 5)
    {
      tm = 5;
    }

    osDelay(tm);
  }
  /* USER CODE END MainTaskFunc */
}

/* USER CODE BEGIN Header_PowerTaskFunc */
/**
 * @brief Function implementing the powerTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_PowerTaskFunc */
void PowerTaskFunc(void *argument)
{
  /* USER CODE BEGIN PowerTaskFunc */
  /* Infinite loop */
  Int_Power_Init();

  for (;;)
  {
    // 等待电源按键按下
    if (Int_Power_PowerKeyIsPressed())
    {
      // 关闭电源
      Int_Power_PowerOFF();
    }

    // 判断电池是否在充电
    charge = Int_Power_IsCharge();
    // 获取电池电压值
    v = Int_Power_GetV();

    // 采用异步更新，放置多个任务同时对lvgl操作引发的线程不安全问题
    lv_async_call(update_battery_callback, NULL);

    // COM_DEBUG_LN("charge == %d, v == %f", charge ? 1 : 0, v);

    osDelay(10);
  }
  /* USER CODE END PowerTaskFunc */
}

/* USER CODE BEGIN Header_GPSTaskFunc */
/**
 * @brief Function implementing the gpsTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_GPSTaskFunc */
void GPSTaskFunc(void *argument)
{
  /* USER CODE BEGIN GPSTaskFunc */
  /* Infinite loop */

  // 存储GPS接收到的数据
  char *str = NULL;
  // 存储经纬度数据是否有效
  char activityFlag = 0;

  Int_ATGM336H_Init();

  // 上一次时间记录
  TickType_t lastTime = xTaskGetTickCount(), currentTime = 0;
  for (;;)
  {
    if (gps_sizes > 0)
    {
      // COM_DEBUG_LN("gps_len: %d", gps_sizes);
      // COM_DEBUG_LN("gps_data: %s", gps_buffers);

      // 解析卫星个数
      str = strstr((char *)gps_buffers, "GNGGA");
      if (str != NULL)
      {
        // $GNGGA,070822.000,4006.81888,N,11621.89413,E,1,05,25.5,30.2,M,-9.6,M,,*58
        sscanf(str, "%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%hhu", &saleNum);
        // COM_DEBUG_LN("num = %hhu", saleNum);
      }

      str = strstr((char *)gps_buffers, "GNRMC");
      if (str != NULL)
      {
        // 解析数据状态
        sscanf(str, "%*[^AV]%c", &activityFlag);
        COM_DEBUG_LN("[[%c]]", activityFlag);

        if (activityFlag == 'A')
        {
          // 有效数据，解析经纬度，经纬度方向，速度
          // $GNRMC,070822.000,A,4006.81888,N,11621.89413,E,0.81,359.02,020624,,,A,V*02
          sscanf(str, "%*[^A]A,%f,%c,%f,%c,%f", &lat, &latDir, &lon, &lonDir, &speed);

          // 将经纬度格式转化为°格式
          // 维度xxyy.yyy xx代表° yy.yyy代表′ 需要转化zz.ppp°
          lat = (int16_t)lat / 100 + (lat - ((int16_t)lat / 100) * 100) / 60;
          // 经度xxxyy.yyy xxx代表° yy.yyy代表′ 需要转化zzz.ppp°
          lon = (int16_t)lon / 100 + (lon - ((int16_t)lon / 100) * 100) / 60;

          // 南维和西经是负值
          if (latDir == 'S')
          {
            lat = -lat;
          }
          if (lonDir == 'W')
          {
            lon = -lon;
          }

          // 固定测试位置：深圳西部硅谷。
          // 这里直接使用用户取点坐标，避免再次转换后红点向右下偏移。
          lat = 22.626121f;
          lon = 113.837692f;

          // 实测当前离线瓦片与该取点坐标直接匹配，不再二次转换。
          // gps_to_gcj02(lat, lon, &lat, &lon);

          // 转换速度的单位
          speed *= 1.852f;

          // 获取当前时间
          currentTime = xTaskGetTickCount();
          // 一轮的骑行时间
          uint32_t sec = (currentTime - lastTime) / 1000;
          // 一轮的骑行里程
          distance += speed * (sec * 1.0 / 3600);
          // 总的骑行时间（s）
          time += sec;
          // 更新上一次时间
          lastTime = currentTime;

          // COM_DEBUG_LN("lon = %f, lonDir = %c, lat = %f, latDir = %c, speed = %f, distance = %f, time = %d", lon, lonDir, lat, latDir, speed, distance, time);
          // 更新数据到lvgl
          lv_async_call(update_bike_cb, NULL);
        }
      }

      // gps_sizes = 0;
    }

    osDelay(1000);
  }
  /* USER CODE END GPSTaskFunc */
}

/* USER CODE BEGIN Header_slopeTaskFunc */
/**
 * @brief Function implementing the slopeTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_slopeTaskFunc */
void slopeTaskFunc(void *argument)
{
  /* USER CODE BEGIN slopeTaskFunc */
  /* Infinite loop */

  Int_LSM6DSM_Init();

  for (;;)
  {
    Int_LSM6DSM_GetAccGyro();

    // 计算出弧度
    float angle = atan2f(-accgyro.acc_y, -accgyro.acc_z);

    // 计算出坡度角
    float angle_deg = angle * 180.0f / 3.1415f;

    // 计算出坡度
    slope_percentage = tanf(angle_deg * 3.1415f / 180.0f) * 100.0f;
    COM_DEBUG_LN("angle_deg = %f slope_percentage = %f", angle_deg, slope_percentage);

    // 更新坡度
    lv_async_call(update_slope, NULL);

    // COM_DEBUG_LN("accX = %d accY = %d accZ = %d gyroX = %d gyroY = %d gyroZ = %d", accgyro.acc_x, accgyro.acc_y, accgyro.acc_z, accgyro.gyro_x, accgyro.gyro_y, accgyro.gyro_z);

    osDelay(20);
  }
  /* USER CODE END slopeTaskFunc */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
