#include "timer_hal.h"
#include "rp2040_timer.h"


void timer_hal_init(timer_hal_t *timerInstance)
{
    switch(timerInstance->sysType) {
        case RP2040:
            timerInstance->init = rp2040_timer_init;
            timerInstance->get_ms = rp2040_timer_get_ms;
            timerInstance->delay_ms = rp2040_timer_delay_ms;
            break;
        default:
            break;
    }

    if(timerInstance->init) {
        timerInstance->init(timerInstance);
    }
} 