#ifndef _PWM_USR_CFG_H
#define _PWM_USR_CFG_H

#include <stdint.h>

typedef struct
{
  uint8_t   stdby; 
  uint16_t  rightDuty;
  uint16_t  leftDuty;
} PWMOutput;

#endif
