#include "motor_control.h"
#include <math.h>
#include <stdio.h>

bool motor_init(MotorController_t *motor) {
    if (!motor || !motor->pwmU || !motor->pwmV || !motor->pwmW || !motor->encoder) return false;
        
    pwm_hal_init(motor->pwmU);
    pwm_hal_init(motor->pwmV);
    pwm_hal_init(motor->pwmW);
    
    motor->pwmU->setFreqHz(motor->pwmU, 50000);
    motor->pwmV->setFreqHz(motor->pwmV, 50000);
    motor->pwmW->setFreqHz(motor->pwmW, 50000);
    
    motor->pwmU->start(motor->pwmU);
    motor->pwmV->start(motor->pwmV);
    motor->pwmW->start(motor->pwmW);

    // Initialize encoder
    if (!encoderHalInit(motor->encoder)) {
        while(1) {printf("failed??");}
        return false;
    }
    
        
    // Set some defaults
    motor->max_duty = 50;
    motor->pole_pairs = 7;
    motor->target_elec_angle = 0;

    return true;
}

void motor_set_max_duty(MotorController_t *motor, uint16_t duty) {
    if (duty > 100) duty = 50;
    motor->max_duty = duty;
}

void motor_set_target_angle(MotorController_t *motor, float mech_angle) {
    // Convert mechanical angle to electrical angle based on pole pairs
    float elec_angle = fmodf(mech_angle * motor->pole_pairs, 360.0f);
    motor->target_elec_angle = elec_angle;
}

void motor_update(MotorController_t *motor) {
    if (!motor) return;

    // Read current mechanical angle
    motor->encoder->read(motor->encoder);
    motor->encoder->process(motor->encoder);
    float mech_angle = motor->encoder->angleDegrees;

    // Calculate electrical angle (you can use target_elec_angle instead for open-loop)
    float elec_angle = fmodf(mech_angle * motor->pole_pairs, 360.0f);
    float theta = elec_angle * (M_PI / 180.0f);

    // Calculate 3-phase sine wave PWM duty ratios
    float Ua = sinf(theta);
    float Ub = sinf(theta - 2.0f * M_PI / 3.0f);
    float Uc = sinf(theta + 2.0f * M_PI / 3.0f);

    // Map from [-1..1] sine to [0..max_duty]
    uint16_t dutyA = (uint16_t)((Ua + 1.0f) * 0.5f * motor->max_duty);
    uint16_t dutyB = (uint16_t)((Ub + 1.0f) * 0.5f * motor->max_duty);
    uint16_t dutyC = (uint16_t)((Uc + 1.0f) * 0.5f * motor->max_duty);

    motor->pwmU->setDuty(motor->pwmU, dutyA);
    motor->pwmV->setDuty(motor->pwmV, dutyB);
    motor->pwmW->setDuty(motor->pwmW, dutyC);
}