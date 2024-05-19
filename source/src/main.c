#include <pico/stdlib.h>
#ifdef USE_SYSVIEW
#include "SEGGER_SYSVIEW_FreeRTOS.h"
#endif
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include "main.h"

/* Private defines */
static int rightCounts = 0, leftCounts = 0;

/* Private constants */
const TickType_t xPeriod_10ms = pdMS_TO_TICKS(10U);
const TickType_t xPeriod_20ms = pdMS_TO_TICKS(20U);

/* Private handlers */
QueueHandle_t xAnglesQueueHandler;
QueueHandle_t xSpeedQueueHandler;
QueueHandle_t xControlQueueHandler;
SemaphoreHandle_t xControlSemaphoreHandler;

/* Function prototypes */
void vCalculateAngles(void *pvParameters);
void vControlAlgorithm(void *pvParameters);
void vTCPIPTransmitAngles(void *pvParameters);
void vApplicationIdleHook(void);
void vISREncoderChannels(uint gpio, uint32_t event_mask);

void picoConfig();
void errorHandler();

int main()
{
  /* Configuration */
  picoConfig();

  /* Queues creation */
  xAnglesQueueHandler = xQueueCreate(5, sizeof(FilteredAngles));
  xSpeedQueueHandler = xQueueCreate(5, sizeof(WheelsSpeed));
  xControlSemaphoreHandler = xQueueCreate(5, sizeof(ControlParameters));

  /* Semaphores creation */

  /* Tasks creation */
  xTaskCreate(vCalculateAngles, "Angles calculation task", configMINIMAL_STACK_SIZE, NULL, 10U, NULL);
  xTaskCreate(vControlAlgorithm, "Set PWM task", configMINIMAL_STACK_SIZE, NULL, 11U, NULL);
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

  xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    /* Every 10ms will wait for task */
    xTaskDelayUntil(&xLastWakeTime, xPeriod_10ms);

    /* Get accelerometer and gyroscope converted to phys dimensions xyz */
    getIMUData(&xIMUData);

    /* Filter y */
    /* Pitch (around y) acceleration estimate m/s2 */
    xTemp_theta = atan2(-xIMUData.accelRaw[0U], xIMUData.accelRaw[2U]);

    /* Pitch angular speed estimate rad/s */
    xTemp_theta_dot = (xAngles.y_ang + xIMUData.gyroRaw[1U] * SAMPLE_S);
    
    xAngles.y_ang = ((xTemp_theta * FILTER_ALPHA) + ((1.0 - FILTER_ALPHA) * xTemp_theta_dot));

    /* Queue filtered angles */
    xQueueSend(xAnglesQueueHandler, &xAngles.y_ang, 0U);
  }
}

void vTCPIPTransmitAngles(void *pvParameters)
{
  TickType_t xLastWakeTime;
  FilteredAngles xAngles;  
  WheelsSpeed xSpeed; 
  ControlParameters xControl;
  char xTxBuffer[] = "";

  xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    /* Every 20ms will wait for task */
    xTaskDelayUntil(&xLastWakeTime, xPeriod_20ms);

    /* Poll for background processes to be done */
    cyw43_arch_poll();

    /* Unqueue filtered angles */
    xQueueReceive(xAnglesQueueHandler, &xAngles.y_ang, portMAX_DELAY);
    xQueueReceive(xSpeedQueueHandler, &xSpeed, portMAX_DELAY);
    xQueueReceive(xControlSemaphoreHandler, &xControl, portMAX_DELAY);

    /* Format angles into a const char buffer */
    sprintf(xTxBuffer, "%3.3f,%2.3f,%2.3f,%2.3f,%2.3f\r\n", xAngles.y_ang, xSpeed.right_speed, xControl.rightSpeedReference, xSpeed.left_speed, xControl.leftSpeedReference);

    /* Send buffer to client */
    sendTcpIp(xTxBuffer);
  }
}

void vControlAlgorithm(void *pvParameters)
{
  TickType_t xLastWakeTime;
  WheelsSpeed xSpeed; 
  ControlParameters xControl;
  PWMOutput xPWM;

  static double tmpSpeedReference = 0.0;
  static int xRightTmpCounts, xleftTmpCounts = 0;
  const double speedKp = 16.0, speedKi = 10.0;

  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    /* Every 10ms will wait for task */
    xTaskDelayUntil(&xLastWakeTime, xPeriod_10ms);

    /* Calculate temporal reference */
    xControl.rightSpeedReference = sin(PI * tmpSpeedReference);
    xControl.leftSpeedReference = sin(PI * tmpSpeedReference);
    tmpSpeedReference += 0.01;

    /* Estimate speed for each wheel from encoder readings */
    xSpeed.right_speed =  (2U * PI * (rightCounts - xRightTmpCounts)) / 
                        (PPR * 0.01);

    xSpeed.left_speed =  (2U * PI * (leftCounts - xleftTmpCounts)) / 
                        (PPR * 0.01);

    xRightTmpCounts = rightCounts;
    xleftTmpCounts = leftCounts;

    /* PI for right wheel*/
    xControl.rightSpeedCorrection = discretePID(xControl.rightSpeedReference, xSpeed.right_speed, 
                                          speedKp, speedKi, 0.0);

    /* PI for left wheel*/
    xControl.leftSpeedCorrection = discretePID(xControl.leftSpeedReference, xSpeed.left_speed, 
                                          speedKp, speedKi, 0.0);


    xPWM.leftDuty = xControl.leftSpeedCorrection;
    xPWM.rightDuty = xControl.rightSpeedCorrection;
    xPWM.stdby = 1U;
    
    /* Set the PWM */
    setPWM(xPWM); 
    
    xQueueSend(xSpeedQueueHandler, &xSpeed, portMAX_DELAY);
    xQueueSend(xControlSemaphoreHandler, &xControl, portMAX_DELAY);
  }
}

void vISREncoderChannels(uint gpio, uint32_t event_mask)
{
  static int rightDirection = 1, leftDirection = 1;

  if((gpio == RIGHT_ENCODER_A_PIN) || (gpio == RIGHT_ENCODER_B_PIN))
  {
    if((gpio == RIGHT_ENCODER_A_PIN) && (rightDirection == -1))
    {
      if((event_mask == GPIO_IRQ_EDGE_RISE) && (gpio_get(RIGHT_ENCODER_B_PIN) == 0U))
      {
        rightDirection = 1;
      }
    }
    else if ((gpio == RIGHT_ENCODER_B_PIN) && (rightDirection == 1))
    {
      if((event_mask == GPIO_IRQ_EDGE_RISE) && (gpio_get(RIGHT_ENCODER_A_PIN) == 0U))
      {
        rightDirection = -1;
      }
    }

    if(rightDirection == 1)
    {
      rightCounts++;
    }
    else
    {
      rightCounts--;
    }
  }
  else
  {
    if((gpio == LEFT_ENCODER_A_PIN) && (leftDirection == -1))
    {
      if((event_mask == GPIO_IRQ_EDGE_RISE) && (gpio_get(LEFT_ENCODER_B_PIN) == 0U))
      {
        leftDirection = 1;
      }
    }
    else if ((gpio == LEFT_ENCODER_B_PIN) && (leftDirection == 1))
    {
      if((event_mask == GPIO_IRQ_EDGE_RISE) && (gpio_get(LEFT_ENCODER_A_PIN) == 0U))
      {
        leftDirection = -1;
      }
    }

    if(leftDirection == 1)
    {
      leftCounts++;
    }
    else
    {
      leftCounts--;
    }
  }
  
}

void picoConfig()
{
  int returnValue = 0;

  stdio_init_all();

  gpio_init(POWER_LED_PIN);
  gpio_init(GENERAL_LED_PIN);
  gpio_set_dir(POWER_LED_PIN, GPIO_OUT);
  gpio_set_dir(GENERAL_LED_PIN, GPIO_OUT);
  gpio_put(POWER_LED_PIN, 0);
  gpio_put(GENERAL_LED_PIN, 0);

  /* Initialize PWM module */
  initPWMModule();

  /* Initialize I2C MPU6050 */
  returnValue = initMPU6050();

  if(returnValue != 2U)
  {
    /* errorHandler(); */
  }

  /* Initialize TCPIP */
  returnValue = initTcpIp();
  if(returnValue != 0U)
  {
    errorHandler();
  }

  gpio_set_irq_enabled_with_callback(RIGHT_ENCODER_A_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &vISREncoderChannels);
  gpio_set_irq_enabled_with_callback(RIGHT_ENCODER_B_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &vISREncoderChannels);
  gpio_set_irq_enabled_with_callback(LEFT_ENCODER_A_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &vISREncoderChannels);
  gpio_set_irq_enabled_with_callback(LEFT_ENCODER_B_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &vISREncoderChannels);

  gpio_put(POWER_LED_PIN, 1);

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
    gpio_put(GENERAL_LED_PIN, 1);
    sleep_ms(300);
    gpio_put(GENERAL_LED_PIN, 0);
    sleep_ms(300);
  };
}