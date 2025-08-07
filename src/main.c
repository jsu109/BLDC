#include <stdio.h>
#include "sysType.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/adc.h"
#include "encoder_hal.h"
#include "gpio_hal.h"
#include "pwm_hal.h"
#include <math.h>
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


PWM_hal_t pwmU = {
    .pwmSettings = {
        .sysType = SYSTYPE,
        .duty = 50,
        .clkDiv = 10,
        .wrap = 249,
        .gpioInst = {
            .settings = {
                .sysType = SYSTYPE,
                .gpioPin = 9,
                .gpioFunction = GPIO_HAL_FUNC_PWM,
                .out = 1,
            },
        },
    }
};
PWM_hal_t pwmV = {
    .pwmSettings = {
        .sysType = SYSTYPE,
        .duty = 50,
        .clkDiv = 10,
        .wrap = 249,
        .gpioInst = {
            .settings = {
                .sysType = SYSTYPE,
                .gpioPin = 8,
                .gpioFunction = GPIO_HAL_FUNC_PWM,
                .out = 1,
            },
        },
    }
};
PWM_hal_t pwmW = {
    .pwmSettings = {
        .sysType = SYSTYPE,
        .duty = 50,
        .clkDiv = 10,
        .wrap = 249,
        .gpioInst = {
            .settings = {
                .sysType = SYSTYPE,
                .gpioPin = 7,
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

GPIO_hal_t nSleep = {
    .settings = {
    .gpioPin = 11,
    .out = 1,
    .sysType = RP2040,
    .gpioFunction = GPIO_HAL_FUNC_SIO
    },
};
GPIO_hal_t nFault = {
    .settings = {
    .gpioPin = 10,
    .out = 0,
    .sysType = RP2040,
    .gpioFunction = GPIO_HAL_FUNC_SIO
    },
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

uint16_t map_angle_to_duty(float angle) {
    const uint16_t min_duty = 1;
    const uint16_t max_duty = 100;
    const float min_angle = 0.0f;
    const float max_angle = 360;

    if (angle < min_angle) angle = min_angle;
    if (angle > max_angle) angle = max_angle;
    
    return min_duty + (angle - min_angle) * (max_duty - min_duty) / (max_angle - min_angle);
}


void commutate(float angle, uint16_t DUTY) {
    uint8_t sector = ((int)(angle / 60.0f)) % 6;

    switch (sector) {
        case 0:
            pwmU.setDuty(&pwmU, DUTY);  // U+
            pwmV.setDuty(&pwmV, 0);     // V-
            pwmW.setDuty(&pwmW, 0);
            break;
        case 1:
            pwmU.setDuty(&pwmU, DUTY);
            pwmV.setDuty(&pwmV, 0);
            pwmW.setDuty(&pwmW, DUTY);
            break;
        case 2:
            pwmU.setDuty(&pwmU, 0);
            pwmV.setDuty(&pwmV, DUTY);
            pwmW.setDuty(&pwmW, DUTY);
            break;
        case 3:
            pwmU.setDuty(&pwmU, 0);
            pwmV.setDuty(&pwmV, DUTY);
            pwmW.setDuty(&pwmW, 0);
            break;
        case 4:
            pwmU.setDuty(&pwmU, DUTY);
            pwmV.setDuty(&pwmV, 0);
            pwmW.setDuty(&pwmW, DUTY);
            break;
        case 5:
            pwmU.setDuty(&pwmU, 0);
            pwmV.setDuty(&pwmV, DUTY);
            pwmW.setDuty(&pwmW, 0);
            break;
    }
}

int main()
{
    stdio_init_all();
    adc_init();
    adc_gpio_init(28);  // Enable ADC function on GPIO28
    gpio_hal_init(&nSleep,&nSleep.settings);
    gpio_hal_init(&nFault,&nFault.settings); //initalise nFault, (input)
    
    bool res = encoderHalInit(&encoder1);
    
    bool res2 = encoderHalInit(&encoder2);

    nSleep.put(&nSleep,1); // enable DRV8317

    pwm_hal_init(&pwmU);
    pwm_hal_init(&pwmV);
    pwm_hal_init(&pwmW);
    pwmU.setDuty(&pwmU,10);
    pwmU.setFreqHz(&pwmU, 50000);
    pwmV.setDuty(&pwmV,10);
    pwmV.setFreqHz(&pwmV, 50000);
    pwmW.setDuty(&pwmW,10);
    pwmW.setFreqHz(&pwmW, 50000);
    // Timer example code - This example fires off the callback after 2000ms
   
    // alarm_id_t alarm_id = add_alarm_in_ms(2000, alarm_callback, &encoder1, false);
  
    float angle = 0;
    while (true) {
        // tight_loop_contents();
        float voltage = read_pot_voltage();
    
        encoder1.read(&encoder1);
        encoder2.read(&encoder2);
        encoder1.process(&encoder1);
        encoder2.process(&encoder2);
            pwmU.start(&pwmU);
            pwmV.start(&pwmV);
            pwmW.start(&pwmW);
        if (voltage <= 1) {
            
            #define POLE_PAIRS 7  // Replace with your actual motor's pole pair count

            float mech_angle = encoder1.angleDegrees;
            printf("mech angle encoder 2 %0.1f\n",mech_angle);
            
            float elec_angle = fmodf(mech_angle * POLE_PAIRS, 360.0f);
            float theta = elec_angle * M_PI / 180.0f;

            float Ua = sinf(theta);
            float Ub = sinf(theta - 2 * M_PI / 3.0f);
            float Uc = sinf(theta + 2 * M_PI / 3.0f);

            // Map [-1,1] to [0,DUTY_MAX]
            #define DUTY_MAX 10
            uint16_t dutyA = (Ua + 1.0f) * 0.5f * DUTY_MAX;
            uint16_t dutyB = (Ub + 1.0f) * 0.5f * DUTY_MAX;
            uint16_t dutyC = (Uc + 1.0f) * 0.5f * DUTY_MAX;

            pwmU.setDuty(&pwmU, dutyA);
            pwmV.setDuty(&pwmV, dutyB);
            pwmW.setDuty(&pwmW, dutyC);


        } else {
            pwmU.stop(&pwmU);
            pwmV.stop(&pwmV);
            pwmW.stop(&pwmW);
        }
        // float freq = map_angle_to_duty(encoder2.angleDegrees);
        // pwmU.setFreqHz(&pwmU, freq);

        
            
        
    
        
    
    }
}
