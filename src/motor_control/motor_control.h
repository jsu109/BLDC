#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include "encoder_hal.h"
#include "pwm_hal.h"

typedef struct {
    // PWM channels for U,V,W phases
    PWM_hal_t *pwmU;
    PWM_hal_t *pwmV;
    PWM_hal_t *pwmW;

    // Encoder instance
    encoderHal_t *encoder;

    // Number of pole pairs
    uint8_t pole_pairs;

    // Current target electrical angle (degrees)
    float target_elec_angle;

    // Maximum duty cycle (0-100)
    uint16_t max_duty;

    float velocity_setpoint;  // desired velocity (degrees per second)
    float velocity_error_integral;  // for integral term accumulation
    float kp_velocity;  // proportional gain
    float ki_velocity;  // integral gain


} MotorController_t;

// Initialize motor controller with PWM and encoder pointers
bool motor_init(MotorController_t *motor);

// Set max duty cycle
void motor_set_max_duty(MotorController_t *motor, uint16_t duty);

// Set mechanical angle target in degrees
void motor_set_target_angle(MotorController_t *motor, float mech_angle);

// Call regularly to update PWM outputs based on encoder and target angle
void motor_update(MotorController_t *motor);

void motor_open_loop_spin(MotorController_t *motor);

void sweep_gains(MotorController_t *motor);

#endif