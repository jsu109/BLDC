#include <stdio.h>
#include "sysType.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "encoder_hal.h"
#include "gpio_hal.h"

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
        .gpioFunction = GPIO_HAL_FUNC_NULL,
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
        .gpioFunction = GPIO_HAL_FUNC_NULL,
    },
    .cs_gpioInst = {0},
};
GPIO_hal_t led;
// GPIO_settings_t led_settings = {
//     .gpioPin = 20,
//     .out = 1,
//     .sysType = RP2040,
//     .gpioFunction = GPIO_HAL_FUNC_NULL
// };


int main()
{
    stdio_init_all();
    
    // gpio_put(led.settings.gpioPin,1);
    
    
    // 
    bool res = encoderHalInit(&encoder1);
    
    bool res2 = encoderHalInit(&encoder2);
    // Timer example code - This example fires off the callback after 2000ms
   
    // alarm_id_t alarm_id = add_alarm_in_ms(2000, alarm_callback, &encoder1, false);
  
    // led.put(&led,1);
    while (true) {
        // tight_loop_contents();
        
        encoder1.read(&encoder1);
        encoder2.read(&encoder2);
        encoder1.process(&encoder1);
        encoder2.process(&encoder2);

        printf("degrees1: %.1f, degrees2: %.1f\n",encoder1.angleDegrees, encoder2.angleDegrees);
            
        
    
        
    
    }
}
