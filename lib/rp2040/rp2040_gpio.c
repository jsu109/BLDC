#include "rp2040_gpio.h"
#include "hardware/gpio.h"
#include <stdio.h>
void rp2040_gpio_init( GPIO_settings_t *settings)
{
    gpio_function_t func = (gpio_function_t)(uintptr_t)(settings->gpioFunction);
    gpio_set_function(settings->gpioPin, func);
    gpio_init(settings->gpioPin);
    gpio_set_dir(settings->gpioPin, settings->out);
    
}

void rp2040_gpio_put(GPIO_settings_t *gpioSettings, bool value)
{
   
    gpio_put(gpioSettings->gpioPin, value);
    printf("[HAL] gpio_put(%d, %d)\n", gpioSettings->gpioPin, value);
    
}