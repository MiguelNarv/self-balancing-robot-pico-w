#ifndef _I2C_H
#define _I2C_H

#include "I2C_cfg.h"

/* Public functions */
extern void initMPU6050();
extern void getRawIMUData(MPU6050_data* data);

#endif 
