#ifndef ADC_HAL_H
#define ADC_HAL_H

#include "sysType.h"
#include "stdint.h"
#include "stdbool.h"
typedef struct  {
    uint8_t adcPin;
    uint8_t input;
    sysType_t sysType;

} adc_settings_t;


typedef struct adc_hal adc_hal_t;
typedef struct adc_hal {
    adc_settings_t settings;
    void (*init)(adc_settings_t *);
    bool (*check_adc_gpio_pin)(uint8_t pin);
    bool (*check_adc_input)(uint8_t input);
    uint16_t (*read)(uint8_t input);
} adc_hal_t;

void adc_hal_init(adc_hal_t *adcInstance);
uint16_t adc_hal_read(adc_hal_t *adcInstance);
#endif //ADC_HAL_H
