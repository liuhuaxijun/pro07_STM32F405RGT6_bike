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

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void MainTaskFunc(void *argument);
void PowerTaskFunc(void *argument);

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

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
