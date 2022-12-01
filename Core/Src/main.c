#include "main.h"
#include "clock.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "lwip.h"
#include "usart.h"
#include "gpio.h"

#include "user_interface.h"
#include "mqtt_client.h"

#include "logger.h"

static void StartDefaultTask(void* argument);

const osThreadAttr_t OledTask_attributes = {
  .name = "OledTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};

const osThreadAttr_t MqttTask_attributes = {
  .name = "MqttTask",
  .stack_size = 2048,
  .priority = (osPriority_t) osPriorityNormal,
};


int main(void)
{
  HAL_Init();

  /* BSP section */
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART3_UART_Init();

  logger_init(&huart3);

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */

  const osThreadAttr_t attributes = {
    .name = "defaultTask",
    .stack_size = 1024,
    .priority = (osPriority_t) osPriorityBelowNormal,
  };

  osThreadNew(StartDefaultTask, NULL, &attributes);

  /* Start scheduler */
  osKernelStart();

  /* Infinite loop */
  while (1)
  {
    /* We should never get here as control is now taken by the scheduler */
  }

}


static void StartDefaultTask(void *argument)
{
  MX_LWIP_Init();

  struct dhcp *dhcp = netif_dhcp_data(&gnetif);
  do {
    osDelay(250);
  } while (dhcp->state != 0x0A);

  osThreadNew(StartUserInterfaceTask, NULL, &OledTask_attributes);
  osThreadNew(StartMqttClientTask, NULL, &MqttTask_attributes);

  for(;;)
  {
    HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    osDelay(500);
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

/* FREERTOS HOOKS */
void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName )
{
  logger_sendMessage("Stack overflow detected on task %s\r\n", pcTaskName);
  while(1){}
}

void vApplicationMallocFailedHook( void )
{
  logger_sendMessage("Memory allocation failed\r\n");
  while(1){}
}
