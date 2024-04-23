/* Control source file
 * 
 * Defines the algorithms used for control.
 */

#include "inc/Control/Control_usr.h"

#define N   20U
#define Ts  0.01

/* Name: discretePID(double Kp, double Ki, double Kd)
 * Arguments: r is the reference, y the measured plant, kp proportional paramenter,
 * ki integral parameter and kd derivative parameter.
 * Description: Calculates the a and b coefficients, then ku and ke. Calculates new
 * error and output and copy them for next iteration. Using backwards Euler.
 */
double discretePID(double r ,double y, double kp, double ki, double kd)
{
  static double e1 = 0.0, e2 = 0.0 , u0 = 0.0, u1 = 0.0 , u2 = 0.0; 
  const double a0 = (1.0 + (N * Ts)), a1 = -(2.0 + (N * Ts)), a2 = 1.0;
  double b0, b1, b2, ku1, ku2, ke0, ke1, ke2, e0;

  b0 = (kp * (1.0 + (N * Ts))) + (ki * Ts * (1.0 + (N * Ts))) + (kd * N);
  b1 = -((kp * (2.0 + (N * Ts))) + (ki * Ts) + (2.0 * kd * N));
  b2 = kp + (kd * N);

  ku1 = a1/a0;
  ku2 = a2/a0; 
  ke0 = b0/a0; 
  ke1 = b1/a0; 
  ke2 = b2/a0;

  e0 = r - y;

  u0  = -(ku1 * u1) - (ku2 * u2) + (ke0 * e0) + (ke1 * e1) + (ke2 * e2);

  e1 = e0;
  u1 = u0;
  u2 = u1;

  return u0;
}
