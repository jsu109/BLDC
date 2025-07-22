#include "gpio_hal.h"
#include "rp2040_gpio.h"



void gpio_hal_init(GPIO_hal_t *gpioInstance, GPIO_settings_t *gpioSettings)
{
    switch(gpioSettings->sysType) {
        case RP2040:
            gpioInstance->init = rp2040_gpio_init;
            break;
        default:
            break;
    }

    if(gpioInstance->init) {
        gpioInstance->init(gpioSettings);
    }
} 

void gpio_hal_set(GPIO_hal_t *gpioInstance, GPIO_settings_t *gpioSettings, bool val)
{
    switch(gpioSettings->sysType) {
        case RP2040:
            gpioInstance->put = rp2040_gpio_put;
            break;
        default:
            break;
    }

    if(gpioInstance->put) {
        gpioInstance->put(gpioSettings,val);
    }
} 