#include "adc_hal.h"
#include "rp2040_adc.h"
#include "stdint.h"

void adc_hal_init(adc_hal_t *adcInstance) {
        switch(adcInstance->settings.sysType) {
        case RP2040:


            adcInstance->check_adc_gpio_pin = rp2040_check_adc_gpio_pin;
            adcInstance->check_adc_input = rp2040_check_adc_input;
            adcInstance->init = rp2040_adc_init;
            adcInstance->read = rp2040_adc_read;

            break;
        default:
            break;
    
//    if (adcInstance->init) {
//        adcInstance->check_adc_gpio_pin(adcInstance->settings.adcPin);
        adcInstance->init(&adcInstance->settings);
    	}	
//	}
}

uint16_t adc_hal_read(adc_hal_t *adcInstance) {
	
        switch(adcInstance->settings.sysType) {
	case RP2040:
		adcInstance->read(adcInstance->settings.input);
		break;
	default:
		break;
	}
}
