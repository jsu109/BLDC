#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include "encoder_hal.h"
#include "pwm_hal.h"

// Control loop parameters
#define CONTROL_PERIOD_MS    1           // main loop period in ms
#define MAX_DUTY             50        // maximum PWM value
#define MIN_PHASE_DUTY       10          // minimum duty to overcome friction/startup
#define KP_VEL               0.5f       // velocity proportional gain
#define KI_VEL               0.001f      // velocity integral gain
#define INTEGRAL_MAX         50.0f       // max integrator value for velocity PI
#define Iq_SLEW_DPS          200.0f      // max Iq change per second
#define VEL_FILTER_ALPHA     0.2f        // velocity low-pass filter coefficient

typedef struct {
    // Velocity control
    float measured_velocity_dps;   // deg/sec
    float target_velocity_dps;     // desired velocity command
    float velocity_target_dps;     // final target (after ramping/smoothing)
    float last_angle_deg;          // last mechanical angle for velocity calculation

    // Electrical angle
    float theta_elec;              // electrical angle (deg)

    // Velocity PI
    float integral;                // integrator state
    float Iq_max;                  // max Iq command in duty units
    float Iq_last;                 // last applied Iq for slew limiting

    // Motor configuration
    uint8_t pole_pairs;            // motor pole pairs
    uint8_t elec_offset;           // electrical offset (deg)
    bool encoder_reversed;         // reverse encoder direction

    // Hardware interfaces
    PWM_hal_t *pwmU;
    PWM_hal_t *pwmV;
    PWM_hal_t *pwmW;
    encoderHal_t *encoder;         // associated encoder
} MotorController_t;

// Initialization
bool motor_init(MotorController_t *motor);
void motor_set_max_duty(MotorController_t *motor, uint16_t duty);

// Open-loop control for testing
void motor_step_lock_test(MotorController_t *motor, float duty_percent);
void motor_open_loop_spin(MotorController_t *motor, float target_rpm, float duty_percent);

// Velocity measurement and control
void motor_update_velocity(MotorController_t *motor, float dt_s);
void motor_velocity_control(MotorController_t *motor, float target_velocity_dps, float dt_s);

// FOC commutation
void commutate_sinusoidal(MotorController_t *motor, float elec_angle_deg, float Iq);

// Calibration & startup routines
void motor_calibrate_offset(MotorController_t *motor, float test_duty);
void motor_startup(MotorController_t *motor, float target_velocity_dps);

// Generic control update
void motor_update(MotorController_t *motor);

// Optional: gain sweep utility
void sweep_gains(MotorController_t *motor);

#endif // MOTOR_CONTROL_H