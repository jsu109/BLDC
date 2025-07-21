// motor_hal_c
#include <stdint.h>
#include "motor_hal.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

// setup phase structure.
phase_t motor_hal_init_phase(uint8_t gpio)
{
    phase_t phase;
    phase.GPIO = gpio;
    phase.sliceNum = pwm_gpio_to_slice_num(gpio);
    return phase;
}

// Initialize PWM for a given motor phase.
void motor_hal_pwm_init(phase_t phase) 
{
    gpio_set_function(phase.GPIO, GPIO_FUNC_PWM);

}

// Set duty cycle for a motor phase in the range [0.0, 1.0].
void motor_hal_set_pwm(uint8_t phase, float duty_cycle);

// Enable PWM output for a phase.
void motor_hal_pwm_enable(phase_t phase) 
{
    pwm_set_enabled(phase.sliceNum, true);
}

// Disable PWM output for a phase.
void motor_hal_pwm_disable(uint8_t phase);