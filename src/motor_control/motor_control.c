#include "motor_control.h"
#include "motor_hal.h"



void init(uint8_t PHASE_PWM_GPIO)
{
    phase_t phasePWM = motor_hal_phase_cfg(PHASE_PWM_GPIO);
    motor_hal_pwm_init(phasePWM);
    
}