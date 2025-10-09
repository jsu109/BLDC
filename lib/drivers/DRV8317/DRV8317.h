#ifndef DRV8317_H
#define DRV8317_H

#include "gpio_hal.h"

typedef struct {

    // ADC_hal_t 
    // GPIO_hal_t csA_gpioInst;
    // GPIO_hal_t csB_gpioInst;
    // GPIO_hal_t csC_gpioInst;

    GPIO_hal_t gain_gpioInst;

} CSA_settings_t;

void drv8317_initCurrentFb( CSA_settings_t* csa_settings);

#endif // DRV8317_h