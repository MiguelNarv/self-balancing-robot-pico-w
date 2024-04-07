#ifndef _I2C_H
#define _I2C_H

#include "I2C_usr_cfg.h"

/* Public functions */
extern int initMPU6050();
extern void getIMUData(MPU6050_data* data);

#endif 
