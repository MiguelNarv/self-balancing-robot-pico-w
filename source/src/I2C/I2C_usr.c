/* I2C source file
 * 
 * Specific to MPU6050, initializes the module and implements a polling function to
 * get the gyroscope and accelerometer values from it.
 * Based on pico-examples/I2C/mpu6050_i2c
 */

#include "inc/I2C/I2C_usr.h"
#include <pico/stdlib.h>
#include <pico/binary_info.h>
#include <hardware/i2c.h>

/* Private definitions */
#define ADDRESS 0X68U
#define I2C_CONTROLLER i2c_default /* 12C0 controller */
#define SDA_PIN PICO_DEFAULT_I2C_SDA_PIN /* GP4*/
#define SCL_PIN PICO_DEFAULT_I2C_SCL_PIN /* GP5*/
#define I2C_FREQ 100000U


int initMPU6050()
{
  /* Register and value constants */
  const uint8_t reset[] = {0x6B, 0x80}, wake[] = {0x6B, 0x08};
  int returnValue = 0;

  /* Do setup of controller and pins */
  i2c_init(I2C_CONTROLLER, I2C_FREQ);
  gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(SDA_PIN);
  gpio_pull_up(SCL_PIN);

  /* Make the I2C pins available to picotool */
  bi_decl(bi_2pins_with_func(SDA_PIN, SCL_PIN, GPIO_FUNC_I2C));

  /* Configure MPU6050 and reset internal regs */
  returnValue = i2c_write_blocking(I2C_CONTROLLER, ADDRESS, reset, sizeof(reset), false);
  sleep_ms(100U);

  /* Wake up mpu6050 from sleep mode (caused by reset) */
  /* Disable temperature measurement */
  returnValue = i2c_write_blocking(I2C_CONTROLLER, ADDRESS, wake, sizeof(wake), false);
  sleep_ms(100U);

  return returnValue;
}

void getRawIMUData(MPU6050_data *data)
{
  /* Register and conversion constants */
  const uint8_t acc_reg = 0x3B, gyr_reg = 0x43;
  const double acc_scaling = 9.81 / 16384.0, gyr_scaling = 250.0 / 32768.0;
  uint8_t tempBuffer[6];

  /* Request and receive accelerometer current values */
  i2c_write_blocking(I2C_CONTROLLER, ADDRESS, &acc_reg, sizeof(acc_reg), true);
  i2c_read_blocking(I2C_CONTROLLER, ADDRESS, tempBuffer, sizeof(tempBuffer), false);

  for (uint8_t i = 0U; i < 3U; i++)
  {
    /* Parse and scale xyz values from byte to m/s^2 */
    data->accelRaw[i] = ((int16_t)(tempBuffer[i * 2U] << 8U | tempBuffer[(i * 2U) + 1U])) * acc_scaling;
  }

   /* Request and receive gyroscope current values */
  i2c_write_blocking(I2C_CONTROLLER, ADDRESS, &gyr_reg, sizeof(acc_reg), true);
  i2c_read_blocking(I2C_CONTROLLER, ADDRESS, tempBuffer, sizeof(tempBuffer), false);

  for (uint8_t i = 0U; i < 3U; i++)
  {
    /* Parse and scale xyz values from byte to deg/s */
    data->gyroRaw[i] = ((int16_t)(tempBuffer[i * 2U] << 8U | tempBuffer[(i * 2U) + 1U])) * gyr_scaling;
  }
}