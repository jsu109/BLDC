#ifndef RP2040_TIMER_H
#define RP2040_TIMER_H
#include "timer_hal.h"


void rp2040_timer_init(timer_hal_t *timerInst);
uint32_t rp2040_timer_get_ms(timer_hal_t *timerInst);
void rp2040_timer_delay_ms(timer_hal_t *timerInst, uint32_t ms);



#endif //RP2040_TIMER_H