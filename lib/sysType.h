#ifndef SYSTYPE_H
#define SYSTYPE_H

typedef enum {
    RP2040,
    STM32
} sysType_t;

#define SYSTYPE RP2040
// #define SYSTYPE STM32;

#define PWM_CLOCK_HZ 125000000.0f  // Default system clock for PWM

#endif
