#include "rp2040_gpio.h"
#include "hardware/gpio.h"

void rp2040_gpio_init( GPIO_settings_t *settings)
{
    gpio_function_t func = (gpio_function_t)(uintptr_t)(settings->gpioFunction);
    gpio_set_function(settings->gpioPin, func);
    
}

