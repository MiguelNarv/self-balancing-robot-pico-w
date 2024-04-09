#include <pico/stdlib.h>
#ifdef USE_SYSVIEW
#include "SEGGER_SYSVIEW_FreeRTOS.h"
#endif
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "main.h"

/* Private defines */

/* Private constants */
const TickType_t xPeriod_10ms = pdMS_TO_TICKS(10U);
const TickType_t xPeriod_20ms = pdMS_TO_TICKS(20U);

/* Private handlers */
QueueHandle_t xAnglesQueueHandler;

/* Function prototypes */
void vCalculateAngles(void *pvParameters);
void vTCPIPTransmitAngles(void *pvParameters);
void vApplicationIdleHook(void);

void picoConfig();
void errorHandler();

int main()
{
  /* Configuration */
  picoConfig();

  /* Tasks creation */
  xTaskCreate(vCalculateAngles, "Angles calculation task", configMINIMAL_STACK_SIZE, NULL, 10U, NULL);
  xTaskCreate(vTCPIPTransmitAngles, "TCPIP send angles task", configMINIMAL_STACK_SIZE, NULL, 9U, NULL);

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

void vCalculateAngles(void *pvParameters)
{
  TickType_t xLastWakeTime;
  MPU6050_data xIMUData;
  static FilteredAngles xAngles; 
  double xTemp_theta = 0.0, xTemp_theta_dot = 0.0;
  const double SAMPLE_S = 0.010, FILTER_ALPHA = 0.02;

  xAnglesQueueHandler = xQueueCreate(5, sizeof(FilteredAngles));
  xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    /* Every 10ms will wait for task */
    xTaskDelayUntil(&xLastWakeTime, xPeriod_10ms);

    /* Get accelerometer and gyroscope converted to phys dimensions xyz */
    getIMUData(&xIMUData);

    /* Filter y */
    /* Pitch (around y) acceleration estimate m/s2 */
    xTemp_theta = atan2(-xIMUData.accelRaw[0U], xIMUData.accelRaw[2U]) * RAD_TO_DEG;

    /* Pitch angular speed estimate rad/s */
    xTemp_theta_dot = (xAngles.y_ang + xIMUData.gyroRaw[1U] * SAMPLE_S);
    
    xAngles.y_ang = ((xTemp_theta * FILTER_ALPHA) + ((1.0 - FILTER_ALPHA) * xTemp_theta_dot));

    /* Queue filtered angles */
    xQueueSend(xAnglesQueueHandler, &xAngles.y_ang, portMAX_DELAY);
  }
}

void vTCPIPTransmitAngles(void *pvParameters)
{
  TickType_t xLastWakeTime;
  FilteredAngles xAngles;  
  char txBuffer[] = "";

  xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    /* Every 20ms will wait for task */
    xTaskDelayUntil(&xLastWakeTime, xPeriod_20ms);

    /* Poll for background processes to be done */
    cyw43_arch_poll();

    /* Unqueue filtered angles */
    xQueueReceive(xAnglesQueueHandler, &xAngles.y_ang, portMAX_DELAY);

    /* Format angles into a const char buffer */
    sprintf(txBuffer, "%3.3f\r\n", xAngles.y_ang);

    /* Send buffer to client */
    sendTcpIp(txBuffer);
  }
}

void picoConfig()
{
  int returnValue = 0;

  stdio_init_all();

  /* Initialize I2C MPU6050 */
  returnValue = initMPU6050();
  if(returnValue != 2U)
  {
    errorHandler();
  }

  /* Initialize TCPIP */
  returnValue = initTcpIp();
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
    /* ToDo */
  };
}