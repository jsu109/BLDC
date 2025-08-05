
#include "pwm_hal.h"
#include "rp2040_pwm.h"

void pwm_hal_init(PWM_hal_t *pwmHalInst, PWM_settings_t *pwmSettings)
{   
    
    switch(pwmHalInst->pwmSettings.sysType) {
        case RP2040:
            pwmHalInst->init = rp2040_pwm_init;
            
            break;
        default:
            break;
    }

    if (pwmHalInst->init) {
        pwmHalInst->init(pwmHalInst);
    }
}