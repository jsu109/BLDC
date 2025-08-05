#ifndef PWM_HAL_H
#define PWM_HAL_H

#include "sysType.h"
#include "gpio_hal.h"
typedef unsigned int uint;

typedef struct {
    sysType_t sysType;
    GPIO_hal_t gpioInst;
    float clkDiv;
    uint16_t wrap;
    uint16_t duty;
    uint8_t hw_handle;
} PWM_settings_t;

typedef struct PWM_hal PWM_hal_t;
typedef struct PWM_hal {
    void (*init)(PWM_hal_t *);
    void (*start)(PWM_hal_t *);
    void (*setDuty)(PWM_hal_t *, uint16_t duty);
    void (*stop)(PWM_hal_t *);
    void (*setFreqKHz)(PWM_hal_t, uint16_t frequency);
    PWM_settings_t pwmSettings;

    // Optional hardware-specific runtime cache
    struct {
        uint sliceNum;
        uint channel;
    } hw;
} PWM_hal_t;

void pwm_hal_init(PWM_hal_t *pwmHalInst);
#endif //PWM_HAL_H