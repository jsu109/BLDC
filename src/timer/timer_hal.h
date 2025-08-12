#ifndef TIMER_HAL_H
#define TIMER_HAL_H

#include "sysType.h"
#include <stdint.h>
typedef struct timer_hal timer_hal_t;
typedef struct timer_hal {
    sysType_t sysType;
    void(*init)(timer_hal_t*);
    uint32_t(*get_ms)();
    void (*delay_ms)(timer_hal_t*, uint32_t ms);
} timer_hal_t;


void timer_hal_init(timer_hal_t *timerInst);
#endif //TIMER_HAL_H

