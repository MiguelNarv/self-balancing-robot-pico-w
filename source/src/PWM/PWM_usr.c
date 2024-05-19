/* PWM source file
 * 
 * Specific to TB6612FNG, initializes pins and implements a function to set duties on
 * both wheels.
 */

#include "inc/PWM/PWM_usr.h"
#include <stdbool.h>
#include <math.h>
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

#define TOP_DUTY_CYCLE      1023U
#define MID_DUTY_CYCLE      511U

/* Name: initPWMModule()
 * Description: Initializes used pins and configures pwm.
 */
void initPWMModule()
{
  int rightSlice, leftSlice; 
  pwm_config rightConfig, leftConfig;

  /* Set pin function */
  gpio_init(H_BRIGDE_STDBY_PIN);
  gpio_init(RIGHT_IN1_PIN);
  gpio_init(RIGHT_IN2_PIN);
  gpio_init(LEFT_IN1_PIN);
  gpio_init(LEFT_IN2_PIN);

  gpio_set_dir(H_BRIGDE_STDBY_PIN, GPIO_OUT);
  gpio_set_dir(RIGHT_IN1_PIN, GPIO_OUT);
  gpio_set_dir(RIGHT_IN2_PIN, GPIO_OUT);
  gpio_set_dir(LEFT_IN1_PIN, GPIO_OUT);
  gpio_set_dir(LEFT_IN2_PIN, GPIO_OUT);

  gpio_put(H_BRIGDE_STDBY_PIN, 0);
  gpio_put(RIGHT_IN1_PIN, 0);
  gpio_put(RIGHT_IN2_PIN, 0);
  gpio_put(LEFT_IN1_PIN, 0);
  gpio_put(LEFT_IN2_PIN, 0);

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
  pwm_config_set_wrap(&rightConfig, TOP_DUTY_CYCLE); 
  pwm_config_set_wrap(&leftConfig, TOP_DUTY_CYCLE);

  /* Init pwm */
  pwm_init(rightSlice, &rightConfig, true);
  pwm_init(leftSlice, &leftConfig, true);
}

/* Name: setPWM(PWMOutput values)
 * Arguments: values is a structure holding stdby and both wheels duties.
 * Description: Sets stdby pin, wheels direction and its duty cicle.
 */
void setPWM(PWMOutput values)
{
  gpio_put(H_BRIGDE_STDBY_PIN, values.stdby);

  if(values.rightDuty > 0.0)
  {
    gpio_put(RIGHT_IN1_PIN, 0U);
    gpio_put(RIGHT_IN2_PIN, 1U);
  }
  else
  {
    gpio_put(RIGHT_IN1_PIN, 1U);
    gpio_put(RIGHT_IN2_PIN, 0U);
  }

  if(values.leftDuty > 0.0)
  {
    gpio_put(LEFT_IN1_PIN, 0U);
    gpio_put(LEFT_IN2_PIN, 1U);
  }
  else
  {
    gpio_put(LEFT_IN1_PIN, 1U);
    gpio_put(LEFT_IN2_PIN, 0U);
  }
  
  if(fabs(values.rightDuty) > TOP_DUTY_CYCLE)
  {
    values.rightDuty = TOP_DUTY_CYCLE;
  }

  if(fabs(values.leftDuty) > TOP_DUTY_CYCLE)
  {
    values.leftDuty = TOP_DUTY_CYCLE;
  }

  pwm_set_gpio_level(RIGHT_PWM_PIN, fabs(values.rightDuty));
  pwm_set_gpio_level(LEFT_PWM_PIN, fabs(values.leftDuty));
}