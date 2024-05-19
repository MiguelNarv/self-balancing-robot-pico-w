#ifndef _MAIN_H
#define _MAIN_H

#include <math.h>
#include "I2C/I2C_usr.h"
#include "TCPIP/TCPIP_usr.h"
#include "PWM/PWM_usr.h"
#include "Control/Control_usr.h"
#include "pico/cyw43_arch.h"

/* Private definitions */
#define RIGHT_ENCODER_A_PIN 18U
#define RIGHT_ENCODER_B_PIN 19U
#define LEFT_ENCODER_A_PIN  20U
#define LEFT_ENCODER_B_PIN  21U
#define POWER_LED_PIN       0U
#define GENERAL_LED_PIN     1U
#define POS_COMPENSATION    6.05


#define RAD_TO_DEG          57.29577951
#define PPR                 1632U
#define PI                  3.14159265

/* Private types */
typedef struct
{
  double x_ang;
  double y_ang;
  double z_ang;
} FilteredAngles;

typedef struct
{
  double right_speed;
  double left_speed;
} WheelsSpeed;
/* Private constants */

#endif
