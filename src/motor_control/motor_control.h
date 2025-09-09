#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include "encoder_hal.h"
#include "pwm_hal.h"

#define CONTROL_PERIOD_MS 1
#define MAX_DUTY 50         // maximum allowed PWM
#define MIN_EFFECTIVE_DUTY 10  // minimum duty to overcome friction
#define KP_VEL 0.4
#define KI_VEL 0.001

typedef struct {
    float velocity_dps;   // deg/sec
    float last_angle_deg;
    float integral;
    uint8_t pole_pairs;
    uint8_t elec_offset;
    PWM_hal_t *pwmU;
    PWM_hal_t *pwmV;
    PWM_hal_t *pwmW;
    // Encoder instance
    encoderHal_t *encoder;
} MotorController_t;

// Initialize motor controller with PWM and encoder pointers
bool motor_init(MotorController_t *motor);

// Set max duty cycle
void motor_set_max_duty(MotorController_t *motor, uint16_t duty);

// Set mechanical angle target in degrees
void motor_set_target_angle(MotorController_t *motor, float mech_angle);

// test code
void motor_step_lock_test(MotorController_t *motor, float duty_percent);

void motor_open_loop_spin(MotorController_t *motor, float target_rpm, float duty_percent);

// Call regularly to update PWM outputs based on encoder and target angle
void motor_update(MotorController_t *motor);

void sweep_gains(MotorController_t *motor);


void motor_update_velocity(MotorController_t *motor, float dt_s);
void motor_velocity_control(MotorController_t *motor, float target_dps, float duty_max,float dt_s);

#endif