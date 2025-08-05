#include <stdio.h>
#include "sysType.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/adc.h"
#include "encoder_hal.h"
#include "gpio_hal.h"
#include "pwm_hal.h"
// int64_t alarm_callback(alarm_id_t id, void *user_data) {
//     encoderHal_t encoder = *((encoderHal_t *)user_data);
//     uint16_t raw = encoder.read();
//     uint16_t angle = raw & 0x3FFF;
//     uint8_t error = (raw >> 14) & 0x1;
//      float degrees;
//     degrees = encoder.process(angle);
//     printf("degrees:%.2f\n",degrees);
    
//     return 0;
// }


PWM_hal_t pwm1 = {
    .pwmSettings = {
        .sysType = SYSTYPE,
        .duty = 50,
        .clkDiv = 6250,
        .wrap = 9999,
        .gpioInst = {
            .settings = {
                .sysType = SYSTYPE,
                .gpioPin = 25,
                .gpioFunction = GPIO_HAL_FUNC_PWM,
                .out = 1,
            },
        },
    }
};
encoderHal_t encoder1 = {
    .id = encoder_ID_AS5048A,

    .comm.spi = {
        .spiInst = {0},
        .spiData = {0},
        .spiSettings = {
            .sysType = SYSTYPE,
            .baudrate = 500000,  // 500 kHz
            .dataBits = 16,
            .order = 1,
            .cpol = 0,
            .cpha = 1,
            .hw_handle = 0       // SPI0
        },
    },

    .cs_gpioSettings = {
        .sysType = SYSTYPE,
        .gpioPin = 17,
        .out = 1,
        .gpioFunction = GPIO_HAL_FUNC_SIO,
    },
    .cs_gpioInst = {0},
};
encoderHal_t encoder2 = {
    .id = encoder_ID_AS5048A,
    //spi for comms
    .comm.spi = {
        .spiInst = {0},
        .spiData = {0},
        .spiSettings = {
            .sysType = SYSTYPE,
            .baudrate = 500000,  // 500 kHz
            .dataBits = 16,
            .order = 1,
            .cpol = 0,
            .cpha = 1,
            .hw_handle = 0       // SPI0
        },
    },

    .cs_gpioSettings = {
        .sysType = SYSTYPE,
        .gpioPin = 20,
        .out = 1,
        .gpioFunction = GPIO_HAL_FUNC_SIO,
    },
    .cs_gpioInst = {0},
};
// GPIO_hal_t led;
// GPIO_settings_t led_settings = {
//     .gpioPin = 25,
//     .out = 1,
//     .sysType = RP2040,
//     .gpioFunction = GPIO_HAL_FUNC_NULL
// };

float read_pot_voltage(void) {
    const float VREF = 3.3f;  // Reference voltage
    const uint16_t MAX_ADC = 4095;  // 12-bit ADC on the RP2040

    adc_select_input(2);  // Select ADC input 2 (GPIO28)
    uint16_t raw = adc_read();  // Read raw ADC value (0–4095)

    return (raw * VREF) / MAX_ADC;  // Convert to voltage
}

// Maps potentiometer voltage (0.0V–3.3V) to frequency (0.1Hz–10Hz) linearly
uint16_t map_voltage_to_duty(float voltage) {
    const uint16_t min_freq = 1;
    const uint16_t max_freq = 100;
    const float min_voltage = 0.0f;
    const float max_voltage = 360;

    if (voltage < min_voltage) voltage = min_voltage;
    if (voltage > max_voltage) voltage = max_voltage;
    
    return min_freq + (voltage - min_voltage) * (max_freq - min_freq) / (max_voltage - min_voltage);
}

int main()
{
    stdio_init_all();
    adc_init();
    adc_gpio_init(28);  // Enable ADC function on GPIO28
    
    
    // 
    bool res = encoderHalInit(&encoder1);
    
    bool res2 = encoderHalInit(&encoder2);
    pwm_hal_init(&pwm1);
    pwm1.setDuty(&pwm1,50000);
    // Timer example code - This example fires off the callback after 2000ms
   
    // alarm_id_t alarm_id = add_alarm_in_ms(2000, alarm_callback, &encoder1, false);
  
    
    while (true) {
        // tight_loop_contents();
        float voltage = read_pot_voltage();
        printf("Potentiometer voltage: %.2f V\n", voltage);
        sleep_ms(500);
        encoder1.read(&encoder1);
        encoder2.read(&encoder2);
        encoder1.process(&encoder1);
        encoder2.process(&encoder2);
        float duty = map_voltage_to_duty(encoder1.angleDegrees);
        pwm1.setDuty(&pwm1, duty);
        

        printf("degrees1: %.1f, degrees2: %.1f\n",encoder1.angleDegrees, encoder2.angleDegrees);
            
        
    
        
    
    }
}
