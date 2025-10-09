
#include "DRV8317.h"


void drv8317_initCurrentFb( CSA_settings_t* csa_settings) {
    
    //set up GPIO instances for gain pin
    gpio_hal_init(&csa_settings->gain_gpioInst, &csa_settings->gain_gpioInst.settings);
}