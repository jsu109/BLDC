#include "rp2040_pwm.h"
#include "hardware/pwm.h"
void rp2040_pwm_init( PWM_hal_t *pwmHalInst) 
{
    PWM_settings_t *settings = &pwmHalInst->pwmSettings;
    GPIO_hal_t *gpioInst = &settings->gpioInst;
    // Initialise GPIO pin for PWM
    gpio_hal_init(gpioInst, &gpioInst->settings);

    // Determine slice and channel
    uint slice = pwm_gpio_to_slice_num(gpioInst->settings.gpioPin);
    uint channel = pwm_gpio_to_channel(gpioInst->settings.gpioPin);

    // Store these for later use (e.g. in setDuty)
    pwmHalInst->hw.sliceNum = slice;
    pwmHalInst->hw.channel = channel;

    // Set clock divider
    pwm_set_clkdiv(slice, settings->clkDiv);

    // Set wrap (TOP value)
    pwm_set_wrap(slice, settings->wrap);

    // Set duty cycle for the correct channel
    pwm_set_chan_level(slice, channel, settings->duty);

    // Enable the PWM slice
    pwm_set_enabled(slice, true);
}

