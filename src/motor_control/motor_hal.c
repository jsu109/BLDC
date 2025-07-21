// motor_hal_c
#include <stdint.h>
#include "motor_hal.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

// configure phase structure.
phase_t motor_hal_phase_cfg(uint8_t gpio)
{
    phase_t phase;
    phase.GPIO = gpio;
    phase.sliceNum = pwm_gpio_to_slice_num(gpio);
    phase.chan = pwm_gpio_to_channel(phase.GPIO);
    phase.wrap = 2499; 
    return phase;
}

// Initialize PWM for a given motor phase.
void motor_hal_pwm_init(phase_t phase) 
{
    gpio_set_function(phase.GPIO, GPIO_FUNC_PWM);

    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, 1.0f);          // Divide by 1
    pwm_config_set_wrap(&cfg, phase.wrap);            // Counter goes from 0 to 2499
    pwm_init(phase.sliceNum, &cfg, false); // false = don't start yet

    pwm_set_chan_level(phase.sliceNum, phase.chan, 0); //set starting duty cycle to 0

}

// Set duty cycle for a motor phase in the range [0.0, 1.0].
void motor_hal_set_pwm(phase_t phase, float duty_cycle)
{
    // Clamp the duty cycle to [0.0, 1.0]
    if (duty_cycle < 0.0f) duty_cycle = 0.0f;
    if (duty_cycle > 1.0f) duty_cycle = 1.0f;
    
    
    uint16_t level = (uint16_t)(duty_cycle * phase.wrap);

    // Set the PWM level for the channel
    pwm_set_chan_level(phase.sliceNum, phase.chan, level);
}

// Enable PWM output for a phase.
void motor_hal_pwm_enable(phase_t phase) 
{
    pwm_set_enabled(phase.sliceNum, true);
}

// Disable PWM output for a phase.
void motor_hal_pwm_disable(phase_t phase)
{
    pwm_set_enabled(phase.sliceNum, false);
}