
#include "pwm_hal.h"
#include "gpio_hal.h"
#include "rp2040_pwm.h"


void pwm_hal_init(PWM_hal_t *pwmHalInst)
{   
    
    switch(pwmHalInst->pwmSettings.sysType) {
        case RP2040:
            pwmHalInst->init = rp2040_pwm_init;
            pwmHalInst->setDuty = rp2040_pwm_setDuty;
            pwmHalInst->setFreqHz = rp2040_pwm_setFreqHz;

            
            break;
        default:
            break;
    }

    if (pwmHalInst->init) {
        gpio_hal_init(&pwmHalInst->pwmSettings.gpioInst, &pwmHalInst->pwmSettings.gpioInst.settings);
        pwmHalInst->init(pwmHalInst);
    }
}

