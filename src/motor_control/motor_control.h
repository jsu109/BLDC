#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>
// planning 
/*This module is a high level module which is hardware independent. will talk only 
to motor_hal.h not directly to GPIO or SPI or PWM. 
This module will achieve the following functionality:
- Initalise Control setup for motor driver eg -call PWM setup routines
- Commutation Logic. 
- speed control. 
- fault handling.
*/

void init(uint8_t PHASE_PWM_GPIO);

#endif