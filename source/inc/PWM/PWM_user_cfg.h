#ifndef _PWM_USR_CFG_H
#define _PWM_USR_CFG_H

#include <stdint.h>

typedef struct
{
  uint8_t   Stdby; 
  uint8_t   Right_in1;  
  uint8_t   Right_in2;  
  uint8_t   Left_in1;   
  uint8_t   Left_in2; 
  uint16_t  Right_duty;
  uint16_t  Left_duty;
} PWM_output;

#endif
