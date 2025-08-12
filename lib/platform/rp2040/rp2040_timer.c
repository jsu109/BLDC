#include "rp2040_timer.h"
#include "hardware/timer.h"
void rp2040_timer_init(timer_hal_t *timerInst)
{

}
uint32_t rp2040_timer_get_ms(timer_hal_t *timerInst)
{
    uint64_t us = time_us_64();
    return (uint16_t)(us / 1000);

}
void rp2040_timer_delay_ms(timer_hal_t *timerInst, uint32_t ms)
{
    busy_wait_ms(ms);
}


