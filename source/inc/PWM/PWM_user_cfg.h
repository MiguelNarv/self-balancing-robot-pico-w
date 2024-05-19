#ifndef _PWM_USR_CFG_H
#define _PWM_USR_CFG_H

#include <stdint.h>

typedef struct
{
  uint8_t   stdby; 
  int16_t  rightDuty;
  int16_t  leftDuty;
} PWMOutput;

#endif
