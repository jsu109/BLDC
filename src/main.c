#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "encoder_hal.h"
#include "gpio_hal.h"

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    return 0;
}


encoderHal_t encoder1;
GPIO_hal_t led;
GPIO_settings_t led_settings = {
    .gpioPin = 25,
    .out = 1,
    .sysType = RP2040
};

int main()
{
    stdio_init_all();
    led.init(&led_settings);
    led.put(&led_settings,1);
    encoder1.id = encoder_ID_AS5048A;
    bool res = encoderHalInit(encoder1.id, &encoder1);
    // Timer example code - This example fires off the callback after 2000ms
    // add_alarm_in_ms(2000, alarm_callback, NULL, false);
    led.put(&led_settings,res);
    while (true) {
        uint16_t angle = encoder1.read();
        float angle_degrees = encoder1.process(angle);

        printf("Angle: %2.0f degrees\n", angle_degrees);
        sleep_ms(20);
        
    
    }
}
