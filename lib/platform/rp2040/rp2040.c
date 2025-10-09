#include "rp2040.h"
#include <stdio.h>



bool check_adc_gpio_pin(uint8_t pin) {
    for (size_t i = 0; i < RP2040_ADC_PIN_COUNT; i++) {
        if (pin == RP2040_ADC_PINS[i]) {
            return true; // valid pin
        }
    }
    printf("Error: Pin %u is not an ADC-capable pin.\n", pin);
    return false; // invalid pin
}