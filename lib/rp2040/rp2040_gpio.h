#ifndef RP2040_GPIO_H
#define RP2040_GPIO_H

#include <stdint.h>
#include "gpio_hal.h"
#include "hardware/gpio.h"


void rp2040_gpio_init( GPIO_settings_t *settings);

void rp2040_gpio_put( GPIO_settings_t *settings, bool val);


#endif 