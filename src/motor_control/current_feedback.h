#ifndef CURRENT_FEEDBACK_H
#define CURRENT_FEEDBACK_H

#include <stdint.h>
#include <stdbool.h>

// Supported feedback sources
typedef enum {
    CURRENT_FB_UNKNOWN = 0,
    CURRENT_FB_MOTOR_DRIVER,   // e.g. DRV8317, TMC6200
    CURRENT_FB_ADC             // Direct ADC current sense
} CurrentFbSource_t;

// Supported motor driver ICs
typedef enum {
    MOTOR_DRIVER_UNKNOWN = 0,
    MOTOR_DRIVER_DRV8317,
    // add more as needed
} MotorDriverId_t;

// Generic 3-phase current in Amps
typedef struct {
    float ia;
    float ib;
    float ic;
    bool valid;
} CurrentFbGeneric_t;

// Abstract interface
typedef struct {
    bool (*init)(void *ctx);
    bool (*read)(void *ctx, CurrentFbGeneric_t *currents);
    void *ctx;   // pointer to driver-specific data
} CurrentFbHal_t;

// Top-level current feedback module
typedef struct {
    CurrentFbSource_t source;   // ADC or motor driver
    MotorDriverId_t driverId;   // Only valid if source == MOTOR_DRIVER
    CurrentFbHal_t hal;         // Function table
} CurrentFeedback_t;

// API
bool current_fb_init(CurrentFeedback_t *currentFb_inst);
bool current_fb_read(CurrentFeedback_t *fb, CurrentFbGeneric_t *currents);

#endif // CURRENT_FEEDBACK_H