#include "sysType.h"
#include "rp2040_pwm.h"
#include "hardware/pwm.h"
#include "stdio.h"
void rp2040_pwm_init( PWM_hal_t *pwmHalInst) 
{
    PWM_settings_t *settings = &pwmHalInst->pwmSettings;
    GPIO_hal_t *gpioInst = &settings->gpioInst;
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

    
    // // Enable the PWM slice
    // pwm_set_enabled(slice, true); dont start

    printf("PWM INIT: slice %d, chan %d, clkDiv %.2f, wrap %u\n",
    slice, channel, settings->clkDiv, settings->wrap);
}



void rp2040_pwm_setFreqHz(PWM_hal_t *pwmHalInst, float freq_hz) {
    const float max_clkdiv = 255.0f;
    const float min_clkdiv = 1.0f;
    const uint32_t clock = PWM_CLOCK_HZ;
    uint slice = pwmHalInst->hw.sliceNum;

    if (freq_hz <= 0) return;

    // Try to use wrap = 65535 for high resolution
    uint32_t wrap = 65535;
    float clkdiv = clock / (freq_hz * (wrap + 1));

    // If clkdiv is out of range, recalculate with constrained clkdiv
    if (clkdiv > max_clkdiv) {
        clkdiv = max_clkdiv;
        wrap = (uint32_t)(clock / (clkdiv * freq_hz)) - 1;
        if (wrap > 65535) wrap = 65535;
    } else if (clkdiv < min_clkdiv) {
        clkdiv = min_clkdiv;
        wrap = (uint32_t)(clock / (clkdiv * freq_hz)) - 1;
        if (wrap > 65535) wrap = 65535;
    }

    pwm_set_wrap(slice, wrap);
    pwm_set_clkdiv(slice, clkdiv);

    pwmHalInst->pwmSettings.wrap = wrap;
    pwmHalInst->pwmSettings.clkDiv = clkdiv;

}

void rp2040_pwm_setDuty(PWM_hal_t *pwmHalInst, uint16_t duty_percent)
{
    uint slice = pwmHalInst->hw.sliceNum;
    uint chan  = pwmHalInst->hw.channel;
    uint16_t wrap = pwmHalInst->pwmSettings.wrap;

    // Clamp to valid range
    if (duty_percent > 100)
        duty_percent = 100;

    // Calculate level based on percentage
    uint16_t level = (duty_percent * (wrap)) / 100;

    // Set duty cycle for the correct channel
    pwm_set_chan_level(slice, chan, level);

    // Update internal record
    pwmHalInst->pwmSettings.duty = duty_percent;
}


void rp2040_pwm_start(PWM_hal_t *pwmHalInst)
{
    uint slice = pwmHalInst->hw.sliceNum;
    pwm_set_enabled(slice, true);
}

void rp2040_pwm_stop(PWM_hal_t *pwmHalInst)
{
    uint slice = pwmHalInst->hw.sliceNum;
    pwm_set_enabled(slice, false);
}
