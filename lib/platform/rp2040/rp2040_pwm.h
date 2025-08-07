#ifndef RP2040_PWM_H
#define RP2040_PWM_H

#include "pwm_hal.h"

void rp2040_pwm_init( PWM_hal_t *pwmHalInst);

void rp2040_pwm_setFreqHz(PWM_hal_t *pwmHalInst, float frequency);

void rp2040_pwm_setDuty(PWM_hal_t *pwmHalInst, uint16_t duty);

#endif //RP2040_PWM_H
