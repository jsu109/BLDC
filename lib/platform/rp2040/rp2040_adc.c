#include "rp2040_adc.h"
#include "hardware/adc.h"
#include "stdio.h"

bool rp2040_check_adc_gpio_pin(uint8_t pin) {
    for (int i = 0; i < RP2040_ADC_PINS_COUNT; i++) {
        if (pin == RP2040_ADC_PINS[i]) {
            return true;
        }
    }
    printf("Error: Pin %u is not an ADC-capable pin.\n", pin);
    return false; // invalid pin
}

bool rp2040_check_adc_input(uint8_t input) {
    for (int i = 0; i < RP2040_ADC_INPUTS_COUNT; i++) {
        if (input == RP2040_ADC_INPUTS[i]) {
            return true;
        }
    }
    printf("Error: input %u is not a valid ADC Input pin.\n",input );
    return false; // invalid pin
}

void rp2040_adc_init(adc_settings_t *adc_settings) {
	
	assert(adc_settings->input < 4);
    	adc_init();
    	adc_gpio_init(adc_settings->adcPin);
    	adc_select_input(adc_settings->input);


}
uint16_t rp2040_adc_read(uint8_t adc_input) {
	adc_select_input(adc_input);
	return adc_read();
}


// adc_init();

    // // Make sure GPIO is high-impedance, no pullups etc
    // adc_gpio_init();
    // // Select ADC input 0 (GPIO26)
    // adc_select_input(0);

    // while (1) {
    //     // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    //     const float conversion_factor = 3.3f / (1 << 12);
    //     uint16_t result = adc_read();
    //     printf("Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor);
    //     sleep_ms(500);
    // }

