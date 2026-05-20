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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "queue.h"
#include "semphr.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
// Semaphore và Queue
SemaphoreHandle_t xSem_FireAlarm;
QueueHandle_t xQueue_ESP;

// Handle các Task
TaskHandle_t xTask_Sensor_Handle;
TaskHandle_t xTask_Battery_LCD_Handle;
TaskHandle_t xTask_FireAlarm_Handle;
TaskHandle_t xTask_ESP_Handle;

/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void vTask_Sensor(void *pvParameters);
void vTask_Battery_LCD(void *pvParameters);
void vTask_FireAlarm(void *pvParameters);
void vTask_ESP(void *pvParameters);
/* USER CODE END FunctionPrototypes */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void vStartFreeRTOSTasks(void)
{
    xSem_FireAlarm = xSemaphoreCreateBinary();
    xQueue_ESP = xQueueCreate(5, sizeof(ESP_Message_t));

    xTaskCreate(vTask_Sensor,       "Task_Sensor",      128, NULL, 3, &xTask_Sensor_Handle);
    xTaskCreate(vTask_Battery_LCD,  "Task_Battery_LCD", 128, NULL, 1, &xTask_Battery_LCD_Handle);
    xTaskCreate(vTask_FireAlarm,    "Task_FireAlarm",   128, NULL, 4, &xTask_FireAlarm_Handle);
    xTaskCreate(vTask_ESP,          "Task_ESP",         128, NULL, 1, &xTask_ESP_Handle);
}

// TASK C: Task_FireAlarm - NHIỆM VỤ CỦA ĐOÀN TRỌNG THÁI
void vTask_FireAlarm(void *pvParameters)
{
    for(;;)
    {
        if(xSemaphoreTake(xSem_FireAlarm, portMAX_DELAY) == pdTRUE)
        {
            // Kích hoạt còi
            HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);

            // Gửi vào Queue để Task_ESP gửi lên Telegram
            ESP_Message_t msg = FIRE_ALARM;
            xQueueSend(xQueue_ESP, &msg, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(fire_alarm_delay));
    }
}

void vTask_Sensor(void *pvParameters)
{
    for(;;)
    {
        // Giả lập phát hiện cháy mỗi 5 giây
        xSemaphoreGive(xSem_FireAlarm);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
void vTask_Battery_LCD(void *pvParameters) { for(;;) { vTaskDelay(pdMS_TO_TICKS(battery_lcd_delay)); } }
void vTask_ESP(void *pvParameters)         { for(;;) { vTaskDelay(pdMS_TO_TICKS(esp_delay)); } }

/* USER CODE END Application */

