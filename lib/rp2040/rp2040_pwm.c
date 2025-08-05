#include "sysType.h"
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



void rp2040_pwm_setFreqKHz(PWM_hal_t *pwmHalInst, uint16_t freq_khz)
{
    uint slice = pwmHalInst->hw.sliceNum;
    uint16_t wrap = pwmHalInst->pwmSettings.wrap;
    float clkdiv;

    // Avoid divide by zero
    if (freq_khz == 0 || wrap == 0) {
        return;
    }

    // Calculate clkdiv
    clkdiv = PWM_CLOCK_HZ / (1000.0f * freq_khz * (wrap + 1));

    // Apply it
    pwm_set_clkdiv(slice, clkdiv);

    // Update internal record
    pwmHalInst->pwmSettings.clkDiv = clkdiv;
}