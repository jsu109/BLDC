#include <stdio.h>
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


encoderHal_t encoder1;
GPIO_hal_t led;
GPIO_settings_t led_settings = {
    .gpioPin = 25,
    .out = 1,
    .sysType = RP2040,
    .gpioFunction = GPIO_HAL_FUNC_NULL
};


int main()
{
    stdio_init_all();
    
    gpio_put(led.settings.gpioPin,1);
    
    encoder1.id = encoder_ID_AS5048A;
    bool res = encoderHalInit(encoder1.id, &encoder1);
    
    // Timer example code - This example fires off the callback after 2000ms
   
    // alarm_id_t alarm_id = add_alarm_in_ms(2000, alarm_callback, &encoder1, false);
  
    // led.put(&led.settings,1);
    while (true) {
        // tight_loop_contents();
        
        uint16_t raw = encoder1.read();
        uint16_t angle = raw & 0x3FFF;
        uint8_t error = (raw >> 14) & 0x1;
        float degrees;
        degrees = encoder1.process(angle);
        printf("degrees:%.2f\n",degrees);
            
        
    
        
    
    }
}
