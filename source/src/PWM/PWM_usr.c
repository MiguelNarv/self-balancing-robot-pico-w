/* PWM source file
 * 
 * Specific to MPU6050, initializes the module and implements a polling function to
 * get the gyroscope and accelerometer values from it.
 * Based on pico-examples/I2C/mpu6050_i2c
 */

#include "inc/PWM/PWM_usr.h"
#include <stdbool.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>

/* Private definitions */
#define H_BRIGDE_STDBY_PIN  9U
#define RIGHT_IN1_PIN       10U
#define RIGHT_IN2_PIN       11U
#define LEFT_IN1_PIN        12U
#define LEFT_IN2_PIN        13U
#define RIGHT_PWM_PIN       14U
#define LEFT_PWM_PIN        15U

/* Name: initPWMModule()
 * Description: Initializes used pins and configures pwm.
 */
void initPWMModule()
{
  int rightSlice, leftSlice; 
  pwm_config rightConfig, leftConfig;

  /* Set pin function */
  gpio_set_function(H_BRIGDE_STDBY_PIN, GPIO_OUT);
  gpio_set_function(RIGHT_IN1_PIN, GPIO_OUT);
  gpio_set_function(RIGHT_IN2_PIN, GPIO_OUT);
  gpio_set_function(LEFT_IN1_PIN, GPIO_OUT);
  gpio_set_function(LEFT_IN2_PIN, GPIO_OUT);

  gpio_set_function(RIGHT_PWM_PIN, GPIO_FUNC_PWM);
  gpio_set_function(LEFT_PWM_PIN, GPIO_FUNC_PWM);

  /* Get slices for each pwm pin */
  rightSlice = pwm_gpio_to_slice_num(RIGHT_PWM_PIN);
  leftSlice = pwm_gpio_to_slice_num(LEFT_PWM_PIN);

  /* Start configuration with defaults */
  rightConfig = pwm_get_default_config();
  leftConfig = pwm_get_default_config();

  /* Divide clk in its MHZ value, resulting in 1 MHz */
  pwm_config_set_clkdiv(&rightConfig, SYS_CLK_MHZ);
  pwm_config_set_clkdiv(&leftConfig, SYS_CLK_MHZ);

  /* Set wrap to 10 bit */
  pwm_config_set_wrap(&rightConfig, 1023U); 
  pwm_config_set_wrap(&leftConfig, 1023U);

  /* Init pwm */
  pwm_init(rightSlice, &rightConfig, true);
  pwm_init(leftSlice, &leftConfig, true);
}