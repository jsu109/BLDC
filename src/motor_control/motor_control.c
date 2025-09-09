#include "motor_control.h"
#include <math.h>
#include <stdio.h>
#include "pico/time.h"

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
        return false; 
    } 
   
    return true;
     
}


void motor_set_target_angle(MotorController_t *motor, float mech_angle) {
    // Convert mechanical angle to electrical angle based on pole pairs
    float elec_angle_deg = fmodf(-mech_angle * motor->pole_pairs + motor->elec_offset, 360.0f);
    if (elec_angle_deg < 0) elec_angle_deg += 360.0f;
}

void motor_step_lock_test(MotorController_t *motor, float duty_percent) {
    for (int angle = 0; angle < 360; angle += 30) {
        float theta = angle * (M_PI / 180.0f);

        float Ua = sinf(theta);
        float Ub = sinf(theta - 2.0f * M_PI / 3.0f);
        float Uc = sinf(theta + 2.0f * M_PI / 3.0f);

        uint16_t dutyA = (uint16_t)((Ua + 1.0f) * 0.5f * duty_percent);
        uint16_t dutyB = (uint16_t)((Ub + 1.0f) * 0.5f * duty_percent);
        uint16_t dutyC = (uint16_t)((Uc + 1.0f) * 0.5f * duty_percent);

        motor->pwmU->setDuty(motor->pwmU, dutyA);
        motor->pwmV->setDuty(motor->pwmV, dutyB);
        motor->pwmW->setDuty(motor->pwmW, dutyC);
        motor->encoder->read(motor->encoder);
        motor->encoder->process(motor->encoder);
        printf("Lock angle %3d°, duties U:%u V:%u W:%u, encoderAngle:%0.1f\n", angle, dutyA, dutyB, dutyC,motor->encoder->angleDegrees);

        sleep_ms(1000);
    }

    // turn off PWM after test
    motor->pwmU->setDuty(motor->pwmU, 0);
    motor->pwmV->setDuty(motor->pwmV, 0);
    motor->pwmW->setDuty(motor->pwmW, 0);
}

void motor_open_loop_spin(MotorController_t *motor, float target_rpm, float duty_percent) {
    static float theta_deg = 0.0f;

    // electrical speed (deg/s) = mech_rpm * pole_pairs * 360 / 60
    float elec_speed_deg_per_sec = (target_rpm * motor->pole_pairs * 360.0f) / 60.0f;

    // step per control period
    float step_deg = elec_speed_deg_per_sec * (CONTROL_PERIOD_MS / 1000.0f);

    // accumulate theta
    theta_deg += step_deg;
    if (theta_deg >= 360.0f) theta_deg -= 360.0f;
    if (theta_deg < 0.0f)    theta_deg += 360.0f;

    // convert to radians for sinf()
    float theta_rad = theta_deg * (M_PI / 180.0f);

    // generate 3-phase sinusoids
    float Ua = sinf(theta_rad);
    float Ub = sinf(theta_rad - 2.0f * M_PI / 3.0f);
    float Uc = sinf(theta_rad + 2.0f * M_PI / 3.0f);

    // map -1..+1 to 0..duty_percent
    uint16_t dutyA = (uint16_t)((Ua + 1.0f) * 0.5f * duty_percent);
    uint16_t dutyB = (uint16_t)((Ub + 1.0f) * 0.5f * duty_percent);
    uint16_t dutyC = (uint16_t)((Uc + 1.0f) * 0.5f * duty_percent);

    // apply to PWM
    motor->pwmU->setDuty(motor->pwmU, dutyA);
    motor->pwmV->setDuty(motor->pwmV, dutyB);
    motor->pwmW->setDuty(motor->pwmW, dutyC);
    motor->encoder->read(motor->encoder);
    motor->encoder->process(motor->encoder);

    // debug
    printf("encoder%.1f deg, theta %.1f deg, duties U:%u V:%u W:%u\n", motor->encoder->angleDegrees,theta_deg, dutyA, dutyB, dutyC);
}


// --- Velocity Update Parameters ---
#define VELOCITY_UPDATE_INTERVAL_S 0.001f  // update every 1 ms
#define VELOCITY_FILTER_ALPHA      0.05f  // exponential smoothing factor (0-1)

void motor_update_velocity(MotorController_t *motor, float dt_s) {
    // Accumulate time
    encoderHal_updateAngle(motor->encoder);
    encoderHal_updateTimestamp(motor->encoder);
    static float vel_timer = 0.0f;
    vel_timer += dt_s;

    if (vel_timer < VELOCITY_UPDATE_INTERVAL_S) {
        // Not enough time elapsed to update velocity
        return;
    }

    // Compute delta angle since last velocity update
    float deltaAngle = motor->encoder->angleDegrees - motor->last_angle_deg;

    // Handle encoder wraparound (0-360 deg)
    if (deltaAngle > 180.0f)  deltaAngle -= 360.0f;
    if (deltaAngle < -180.0f) deltaAngle += 360.0f;

    // Compute raw velocity (deg/sec)
   float  deltaMS = motor->encoder->deltaTimeMs;
   float deltaS = deltaMS/1000;
    float new_velocity = deltaAngle / (deltaMS/1000); //in degrees/s
    printf("deltaTime (s): %.4f\n",deltaS);
    // Exponential smoothing
    motor->velocity_dps = new_velocity;//= VELOCITY_FILTER_ALPHA * new_velocity +
                         // (1.0f - VELOCITY_FILTER_ALPHA) * motor->velocity_dps;

    // Store last angle for next update
    motor->last_angle_deg = motor->encoder->angleDegrees;

    // Reset timer
    vel_timer = 0.0f;
}


void motor_velocity_control(MotorController_t *motor, float target_dps, float duty_max, float dt_s) {
    // 1) Compute error
    float error = target_dps - motor->velocity_dps;

    // 2) PI control
    motor->integral += error * dt_s;
    float output = KP_VEL * error; //+ KI_VEL * motor->integral;

    // 3) Clamp output to max/min
    if (output > duty_max) output = duty_max;
    if (output < -duty_max) output = -duty_max;

    // 4) Apply minimum effective duty for low speed
    if (output > 0 && output < MIN_EFFECTIVE_DUTY) output = MIN_EFFECTIVE_DUTY;
    if (output < 0 && output > -MIN_EFFECTIVE_DUTY) output = -MIN_EFFECTIVE_DUTY;

    // 5) Increment electrical angle based on target velocity
    static float theta_deg = 0.0f; // keep persistent between calls
    theta_deg += target_dps * motor->pole_pairs * dt_s; // small step per loop

    if (output < 0) {
        output = -output;   // magnitude
        theta_deg += 180.0f; // reverse torque
    }

    // Wrap theta to 0-360
    if (theta_deg >= 360.0f) theta_deg -= 360.0f;
    if (theta_deg < 0.0f)    theta_deg += 360.0f;

    // 6) Compute electrical angle in radians
    float theta_rad = theta_deg * (M_PI / 180.0f);

    // generate 3-phase sinusoids
    float Ua = sinf(theta_rad);
    float Ub = sinf(theta_rad - 2.0f * M_PI / 3.0f);
    float Uc = sinf(theta_rad + 2.0f * M_PI / 3.0f);

    // map -1..+1 to 0..duty_percent
    uint16_t dutyA = (uint16_t)((Ua + 1.0f) * 0.5f * output);
    uint16_t dutyB = (uint16_t)((Ub + 1.0f) * 0.5f * output);
    uint16_t dutyC = (uint16_t)((Uc + 1.0f) * 0.5f * output);

    // apply to PWM
    motor->pwmU->setDuty(motor->pwmU, dutyA);
    motor->pwmV->setDuty(motor->pwmV, dutyB);
    motor->pwmW->setDuty(motor->pwmW, dutyC);

    // update encoder
    motor->encoder->read(motor->encoder);
    motor->encoder->process(motor->encoder);

    printf("encoder%.1f deg, theta %.1f deg, duties U:%u V:%u W:%u, output: %0.1f error: %.1f\n",
           motor->encoder->angleDegrees, theta_deg, dutyA, dutyB, dutyC, output, error);
}