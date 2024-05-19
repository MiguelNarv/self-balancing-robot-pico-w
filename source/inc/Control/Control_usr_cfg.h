#ifndef _CONTROL_USR_CFG_H
#define _CONTROL_USR_CFG_H

typedef struct 
{
  double positionReference;
  double positionCorrection;
  double rightSpeedReference;
  double rightSpeedCorrection;
  double leftSpeedReference;
  double leftSpeedCorrection;
} ControlParameters;

#endif
