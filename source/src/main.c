#include <pico/stdlib.h>
#ifdef USE_SYSVIEW
#include "SEGGER_SYSVIEW_FreeRTOS.h"
#endif
#include <FreeRTOS.h>
#include <task.h>
#include "main.h"

/* Private defines */

/* Private constants */
const TickType_t xPeriod_10ms = pdMS_TO_TICKS(10U);

/* Function prototypes */
void vI2CPolling(void *pvParameters);
void vApplicationIdleHook(void);

void picoConfig();
void errorHandler();

int main()
{

  picoConfig();

  /* Tasks creation*/
  xTaskCreate(vI2CPolling, "I2C polling task", configMINIMAL_STACK_SIZE, NULL, 10U, NULL);

  /* Segger initialization */
#ifdef USE_SYSVIEW
  SEGGER_SYSVIEW_Conf();
  SEGGER_SYSVIEW_Start();
#endif

  vTaskStartScheduler();

  while (1)
  {
    /* Your program should never get here */
  };

  return 0;
}

void vI2CPolling(void *pvParameters)
{
  TickType_t xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    /* Every 10ms will wait for task */
    xTaskDelayUntil(&xLastWakeTime, xPeriod_10ms);


  }
}


void picoConfig()
{
  int returnValue = 0;

  stdio_init_all();

  /* Initialize I2C MPU6050 */
  returnValue = initMPU6050();
  if(returnValue != 0U)
  {
    errorHandler();
  }

}

void vApplicationIdleHook(void)
{
  for (;;)
  {
    /* Send CPU to low power mode */
    __WFI();
  }
}

void errorHandler()
{
   while (1)
  {
    
  };
}