#ifndef RP2040_ADC_H
#define RP2040_ADC_H

#include "adc_hal.h"
#include "stdbool.h"

//RP2040 definitions


#pragma once

// define the arrays properly
static const uint8_t RP2040_ADC_PINS[]   = {26, 27, 28, 29};
static const uint8_t RP2040_ADC_INPUTS[] = {0, 1, 2, 3};

// and their sizes
#define RP2040_ADC_PINS_COUNT   (sizeof(RP2040_ADC_PINS) / sizeof(RP2040_ADC_PINS[0]))
#define RP2040_ADC_INPUTS_COUNT (sizeof(RP2040_ADC_INPUTS) / sizeof(RP2040_ADC_INPUTS[0]))


bool rp2040_check_adc_gpio_pin(uint8_t pin);
bool rp2040_check_adc_input(uint8_t input);


void rp2040_adc_init(adc_settings_t *adc_settings);


#endif //RP2040_ADC_H