#include <stdio.h>
#include "sysType.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include "hardware/timer.h"
#include "hardware/adc.h"
#include "encoder_hal.h"
#include "gpio_hal.h"
#include "pwm_hal.h"
#include "adc_hal.h"
#include "timer_hal.h"
#include "motor_control.h"
#include <math.h>
// int64_t alarm_callback(alarm_id_t id, void *user_data) {
//     encoderHal_t encoder = *((encoderHal_t *)user_data);
    // uint16_t raw = encoder2.read();
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
    .timer = {
        .sysType = SYSTYPE,
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

adc_hal_t test_adc = {
	.settings = {
	.adcPin = 28,
	.input = 2,
	.sysType = RP2040,
	},
};



GPIO_hal_t led;
GPIO_settings_t led_settings = {
    .gpioPin = 25,
    .out = 1,
    .sysType = RP2040,
    .gpioFunction = GPIO_HAL_FUNC_NULL
};

float read_pot_voltage(void) {
    const float VREF = 3.3f;  // Reference voltage
    const uint16_t MAX_ADC = 4095;  // 12-bit ADC on the RP2040

    adc_select_input(2);  // Select ADC input 2 (GPIO28)
    uint16_t raw = adc_read();  // Read raw ADC value (0–4095)



    

    //uint16_t raw = test_adc.read(test_adc.settings.input);
    
    return (raw * VREF) / MAX_ADC;  // Convert to voltage
}


uint16_t map_angle_to_duty(float angle) {
    const uint16_t min_duty = 1;
    const uint16_t max_duty = 100;
    const float min_angle = 0.0f;
    const float max_angle = 3.3;

    if (angle < min_angle) angle = min_angle;
    if (angle > max_angle) angle = max_angle;
    
    return min_duty + (angle - min_angle) * (max_duty - min_duty) / (max_angle - min_angle);
}
float map_voltage_to_velocity(float voltage) {
    const float min_vel = -1000;
    const float max_vel = 1000;
    const float min_voltage = 0.0f;
    const float max_voltage = 3.3;

    if (voltage < min_voltage) voltage = min_voltage;
    if (voltage > max_voltage) voltage = max_voltage;
    
    return min_vel + (voltage - min_voltage) * (max_vel - min_vel) / (max_voltage - min_voltage);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "motor_control.h"

#define INPUT_BUF_SIZE 16
#define VELOCITY_STEP 100.0f   // deg/s per arrow press
#define VELOCITY_STEP_SMALL 10.0f   // deg/s per arrow press
static char input_buf[INPUT_BUF_SIZE];
static uint8_t input_pos = 0;

// Escape sequence buffer for arrow keys
static char esc_seq[3];
static uint8_t esc_pos = 0;

void process_user_input(MotorController_t *motor) {
    int c = getchar_timeout_us(0);  // Non-blocking
    if (c == PICO_ERROR_TIMEOUT) return;

    // Handle ESC sequence for arrow keys
    if (esc_pos > 0 || c == 27) {  // ESC received
        esc_seq[esc_pos++] = (char)c;

        if (esc_pos == 3) {  // Full arrow key sequence
            if (esc_seq[0] == 27 && esc_seq[1] == 91) {  // ESC [
                switch (esc_seq[2]) {
                    case 'A': motor->target_velocity_dps += VELOCITY_STEP; break;  // Up
                    case 'B': motor->target_velocity_dps -= VELOCITY_STEP; break;  // Down
                    case 'C': motor->target_velocity_dps += VELOCITY_STEP_SMALL / 2; break; // Right
                    case 'D': motor->target_velocity_dps -= VELOCITY_STEP_SMALL / 2; break; // Left
                }
                printf("Arrow key input -> target_velocity: %.1f deg/s\n", motor->target_velocity_dps);
            }
            esc_pos = 0;  // Reset for next sequence
        }
        return;
    }

    // Handle typed numbers (0-9) and Enter
    if (c >= '0' && c <= '9') {
        if (input_pos < INPUT_BUF_SIZE - 1) {
            input_buf[input_pos++] = (char)c;
            printf("%c", c);  // Echo back
        }
    } else if (c == '\r' || c == '\n') {
        input_buf[input_pos] = '\0';
        if (input_pos > 0) {
            char *endptr;
            float val = strtof(input_buf, &endptr);
            if (endptr != input_buf) {
                motor->target_velocity_dps = val;
                printf("\nTyped input -> target_velocity: %.1f deg/s\n", motor->target_velocity_dps);
            }
        }
        input_pos = 0;  // Reset buffer
    } else if (c == 127 || c == '\b') {  // Handle backspace
        if (input_pos > 0) {
            input_pos--;
            printf("\b \b");  // Erase character
        }
    } else {
        // Ignore other characters
    }
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
MotorController_t motor;

int main()
{
    stdio_init_all();
    adc_init();
    //adc_gpio_init(28);  // Enable ADC function on GPIO28
    gpio_hal_init(&nSleep,&nSleep.settings);
    gpio_hal_init(&nFault,&nFault.settings); //initalise nFault, (input)
    
    test_adc.init(&test_adc.settings);
    
    gpio_hal_init(&led,&led.settings);
    led.put(&led,0);    
	
    
    bool res2 = encoderHalInit(&encoder2);


    motor.pwmU = &pwmU;
    motor.pwmV = &pwmV;
    motor.pwmW = &pwmW;
    motor.encoder = &encoder2;
    motor.pole_pairs = 7;
    motor.elec_offset = 132;
    motor.Vbus = 16;
    nSleep.put(&nSleep,1); // enable DRV8317
    if (!motor_init(&motor)) {
        // while(1) {printf("Motor init failed\n");}
        nSleep.put(&nSleep,0); // enable DRV8317
        return -1;
    } 
    
    

//testing.
    
    
    // Timer example code - This example fires off the callback after 2000ms
   
    // alarm_id_t alarm_id = add_alarm_in_ms(2000, alarm_callback, &encoder1, false);
  
    float angle = 0;
    // motor_calibrate_offset(&motor, 40);
    // motor_startup(&motor, 300);
    
    while (true) {
       
        // Read your input, e.g. pot voltage, map to max duty
        float pot_voltage = read_pot_voltage();
        // uint16_t duty = map_angle_to_duty(pot_voltage);
        // motor_set_max_duty(&motor, duty);
        float desiredVelocity = map_voltage_to_velocity(pot_voltage);
        if (fabsf(desiredVelocity) <= 35) {
            desiredVelocity = 0;
        }
        // printf("desired setpoint: %0.1f\n",desiredVelocity);
        // Update PWM outputs accordingly
        // motor_lock_angle(&motor, 270.0f); 
        
        
        // motor_update(&motor);
        // 1 ms control loop
        // motor.elec_offset = desiredVelocity;
        // printf("elecOffset: %.1f\n", desiredVelocity);
        
        float target_velocity_dps = desiredVelocity;
        float dt_s = CONTROL_PERIOD_MS / 1000.0f;
        process_user_input(&motor)
;        // motor_update_velocity(&motor, dt_s);
        motor_velocity_control(&motor,motor.target_velocity_dps,dt_s);

        sleep_ms(CONTROL_PERIOD_MS);
        // printf("elecOffset %d\n", motor.elec_offset);
        
        // Add delay or do other tasks
        // sleep_ms(5);
    }       
    // while(1) {
    //     sweep_gains(&motor);
    // }
}
