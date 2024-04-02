#include <pico/stdlib.h>
#ifdef USE_SYSVIEW
#include "SEGGER_SYSVIEW_FreeRTOS.h"
#endif
#include <FreeRTOS.h>
#include <task.h>

/* Private defines */
#define LED_PIN_1 17U
#define LED_PIN_2 18U

/* Private constants */
const TickType_t xPeriod_50ms = pdMS_TO_TICKS(50U);
const TickType_t xPeriod_100ms = pdMS_TO_TICKS(100U);

/* Function prototypes */
void vLED1Task(void *pvParameters);
void vLED2Task(void *pvParameters);
void vApplicationIdleHook(void);

void picoConfig();

int main()
{

  picoConfig();

  /* Tasks creation*/
  xTaskCreate(vLED1Task, "LED1toggle", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
  xTaskCreate(vLED2Task, "LED2toggle", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

   /* Segger initialization */
#ifdef USE_SYSVIEW
  SEGGER_SYSVIEW_Conf();
  SEGGER_SYSVIEW_Start();
#endif
  
  vTaskStartScheduler();
  
  while(1)
  {
    // Your program should never get here
  };
  
  return 0;
  
}

void vLED1Task(void *pvParameters)
{
  TickType_t xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  uint8_t state = 0U;

  for (;;)
  {
    /* Every 50ms will wait for task */
    vTaskDelayUntil(&xLastWakeTime, xPeriod_50ms);

    gpio_put(LED_PIN_1, state);

    if(state == 0U)
    {
      state = 1U;
    }
    else
    {
      state = 0U;
    }
  }
}

void vLED2Task(void *pvParameters)
{
  TickType_t xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  uint8_t state = 0U;

  for (;;)
  {
    /* Every 100ms will wait for task */
    vTaskDelayUntil(&xLastWakeTime, xPeriod_100ms);

    gpio_put(LED_PIN_2, state);

    if(state == 0U)
    {
      state = 1U;
    }
    else
    {
      state = 0U;
    }
  }
}

void picoConfig()
{
  stdio_init_all();

  /* LEDs initialization */
  gpio_init(LED_PIN_1);
  gpio_set_dir(LED_PIN_1, GPIO_OUT);
  gpio_put(LED_PIN_1, 0);

  gpio_init(LED_PIN_2);
  gpio_set_dir(LED_PIN_2, GPIO_OUT);
  gpio_put(LED_PIN_2, 0);
}

void vApplicationIdleHook(void)
{
  for (;;)
  {
    /* Send CPU to low power mode */
    __WFI();
  }
}
