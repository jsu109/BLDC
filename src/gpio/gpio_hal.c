#include "gpio_hal.h"
#include "rp2040_gpio.h"

#include <stdio.h>


void gpio_hal_init(GPIO_hal_t *gpioInstance, GPIO_settings_t *gpioSettings)
{
    switch(gpioSettings->sysType) {
        case RP2040:
            gpioInstance->init = rp2040_gpio_init;
            gpioInstance->put = rp2040_gpio_put;
            gpioInstance->settings = *gpioSettings;
            break;
        default:
            break;
    }

    if (gpioInstance->init) {
        gpioInstance->init(&gpioInstance->settings);
    }
    
    
} 

void gpio_hal_put(GPIO_hal_t *gpioInstance, bool val)
{
    
    gpioInstance->put(gpioInstance,val);


} 