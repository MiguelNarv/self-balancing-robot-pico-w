#ifndef _MAIN_H
#define _MAIN_H

#include <math.h>
#include "I2C/I2C_usr.h"
#include "TCPIP/TCPIP_usr.h"


/* Private definitions */
#define RAD_TO_DEG 57.2957795131f

/* Private types */
typedef struct
{
  double x_ang;
  double y_ang;
  double z_ang;
} FilteredAngles;

/* Private constants */


#endif
