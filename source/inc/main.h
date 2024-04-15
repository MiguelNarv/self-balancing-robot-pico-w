#ifndef _MAIN_H
#define _MAIN_H

#include <math.h>
#include "I2C/I2C_usr.h"
#include "TCPIP/TCPIP_usr.h"
#include "pico/cyw43_arch.h"

/* Private definitions */
#define RIGHT_ENCODER_A_PIN 18U
#define RIGHT_ENCODER_B_PIN 19U
#define LEFT_ENCODER_A_PIN  20U
#define LEFT_ENCODER_B_PIN  21U
#define RAD_TO_DEG          57.2957795131f

/* Private types */
typedef struct
{
  double x_ang;
  double y_ang;
  double z_ang;
} FilteredAngles;

/* Private constants */


#endif
