#include "motor_control.h"
#include "motor_hal.h"



void init(phase_t phase)
{
    motor_hal_pwm_init(phase);
    
}