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
        return false; 
    } 
    // Set some defaults
    motor->max_duty = 50;
    motor->pole_pairs = 7;
    motor->target_elec_angle = 0;
    motor->velocity_setpoint = 0.0f;
    motor->velocity_error_integral = 0.0f;
    motor->kp_velocity = 0.03f; // Tune these values
    motor->ki_velocity = 0.01f;
    return true;
     
}

void motor_set_max_duty(MotorController_t *motor, uint16_t duty) {
    if (duty > 100) duty = 80;
    motor->max_duty = duty;
}

void motor_set_target_angle(MotorController_t *motor, float mech_angle) {
    // Convert mechanical angle to electrical angle based on pole pairs
    float elec_angle = fmodf(mech_angle * motor->pole_pairs, 360.0f);
    motor->target_elec_angle = elec_angle;
}

void motor_update(MotorController_t *motor) {
    if (!motor) return;

    // Static variables for open-loop startup and closed-loop mode
    static bool closed_loop_enabled = false;
    static float open_loop_theta = 0.0f;   // radians
    const float open_loop_speed = 0.05f;   // radians per call, adjust for startup speed
    const uint16_t open_loop_duty = 50;    // duty cycle %, increase if motor won't start
    const float VELOCITY_DEADBAND = 0.2f;  // velocity threshold to switch control modes (deg/s)
    const float electrical_offset_deg = motor->elec_offset;// tune this experimentally

    // Read encoder
    motor->encoder->read(motor->encoder);
    encoderHal_updateTimestamp(motor->encoder);
    motor->encoder->process(motor->encoder);
    encoderHal_updateVelocity(motor->encoder);

    float mech_angle = motor->encoder->angleDegrees;
    float raw_velocity = motor->encoder->velocityDegPerSec;

    // Low-pass filter velocity (simple exponential filter)
    static float filtered_velocity = 0;
    const float alpha = 0.05f;  // filter smoothing factor (0-1)
    filtered_velocity = alpha * raw_velocity + (1 - alpha) * filtered_velocity;

    // Apply deadband to velocity
    float velocity = (fabsf(filtered_velocity) < VELOCITY_DEADBAND) ? 0.0f : filtered_velocity;

    // Time delta in seconds
    float dt = motor->encoder->deltaTimeMs * 0.001f;
    if (dt <= 0) dt = 0.001f;  // fallback small dt

    // printf("actual velocity %0.1f\n", velocity);
    // printf("motor velocity set point %0.1f\n", motor->velocity_setpoint);

    if (!closed_loop_enabled) {
        // Open-loop startup: increment internal angle and generate PWM
        open_loop_theta += open_loop_speed;
        if (open_loop_theta > 2.0f * M_PI) open_loop_theta -= 2.0f * M_PI;

        float Ua = sinf(open_loop_theta);
        float Ub = sinf(open_loop_theta - 2.0f * M_PI / 3.0f);
        float Uc = sinf(open_loop_theta + 2.0f * M_PI / 3.0f);

        uint16_t dutyA = (uint16_t)((Ua + 1.0f) * 0.5f * open_loop_duty);
        uint16_t dutyB = (uint16_t)((Ub + 1.0f) * 0.5f * open_loop_duty);
        uint16_t dutyC = (uint16_t)((Uc + 1.0f) * 0.5f * open_loop_duty);

        motor->pwmU->setDuty(motor->pwmU, dutyA);
        motor->pwmV->setDuty(motor->pwmV, dutyB);
        motor->pwmW->setDuty(motor->pwmW, dutyC);

        // Switch to closed loop when velocity is above deadband and setpoint nonzero
        if ((fabsf(velocity) > VELOCITY_DEADBAND) && (motor->velocity_setpoint != 0.0f)) {
            closed_loop_enabled = true;
            motor->velocity_error_integral = 0.0f; // Reset integral
            // printf("Switching to closed loop control\n");
        }

        return; // skip closed loop control in open loop mode
    }

    // Closed-loop velocity PI controller

    float error = motor->velocity_setpoint - velocity;
    

    motor->velocity_error_integral += error * dt;
    if (motor->velocity_error_integral > 100.0f) motor->velocity_error_integral = 100.0f;
    if (motor->velocity_error_integral < -100.0f) motor->velocity_error_integral = -100.0f;

    float control_output = motor->kp_velocity * error + motor->ki_velocity * motor->velocity_error_integral;
    printf("setpoint=%.1f, vel=%.1f, error=%.1f, output=%.1f\n",
        motor->velocity_setpoint, velocity, error, control_output);
    // Startup torque bias if velocity zero but setpoint nonzero
    const float STARTUP_TORQUE = 30.0f;
    if (velocity == 0.0f && motor->velocity_setpoint != 0.0f) {
        if (control_output < STARTUP_TORQUE) control_output = STARTUP_TORQUE;
    }

    if (control_output < 0) control_output = 0;
    if (control_output > motor->max_duty) control_output = motor->max_duty;

    uint16_t duty_limit = (uint16_t)control_output;

    // Calculate electrical angle with offset and convert to radians
    float elec_angle_deg = fmodf(-mech_angle * motor->pole_pairs + electrical_offset_deg, 360.0f);
    if (elec_angle_deg < 0) elec_angle_deg += 360.0f;

    float theta = elec_angle_deg * (M_PI / 180.0f);

    float Ua = sinf(theta);
    float Ub = sinf(theta - 2.0f * M_PI / 3.0f);
    float Uc = sinf(theta + 2.0f * M_PI / 3.0f);

    uint16_t dutyA = (uint16_t)((Ua + 1.0f) * 0.5f * duty_limit);
    uint16_t dutyB = (uint16_t)((Ub + 1.0f) * 0.5f * duty_limit);
    uint16_t dutyC = (uint16_t)((Uc + 1.0f) * 0.5f * duty_limit);

    motor->pwmU->setDuty(motor->pwmU, dutyA);
    motor->pwmV->setDuty(motor->pwmV, dutyB);
    motor->pwmW->setDuty(motor->pwmW, dutyC);
}
void motor_lock_angle(MotorController_t *motor, float elec_angle_deg) {
    if (!motor) return;
    motor->encoder->read(motor->encoder);
    encoderHal_updateTimestamp(motor->encoder);
    motor->encoder->process(motor->encoder);
    encoderHal_updateVelocity(motor->encoder);
// Include electrical offset
    float theta = fmodf(elec_angle_deg + motor->elec_offset, 360.0f) * (M_PI / 180.0f);

    float Ua = sinf(theta);
    float Ub = sinf(theta - 2.0f * M_PI / 3.0f);
    float Uc = sinf(theta + 2.0f * M_PI / 3.0f);

    const uint16_t lock_duty = 60; // strong enough to hold rotor

    uint16_t dutyA = (uint16_t)((Ua + 1.0f) * 0.5f * lock_duty);
    uint16_t dutyB = (uint16_t)((Ub + 1.0f) * 0.5f * lock_duty);
    uint16_t dutyC = (uint16_t)((Uc + 1.0f) * 0.5f * lock_duty);

    motor->pwmU->setDuty(motor->pwmU, dutyA);
    motor->pwmV->setDuty(motor->pwmV, dutyB);
    motor->pwmW->setDuty(motor->pwmW, dutyC);
}

void motor_open_loop_spin(MotorController_t *motor) {
    if (!motor) return;

    // Static variables to keep track of angle
    static float open_loop_theta = 0.0f;   // radians
    const float open_loop_speed = 0.0005f;   // radians per call, adjust for speed
    const uint16_t open_loop_duty = 50;    // duty cycle %, increase if motor won't start

    // Increment angle (wrap around 2*PI)
    open_loop_theta += open_loop_speed;
    if (open_loop_theta > 2.0f * M_PI) {
        open_loop_theta -= 2.0f * M_PI;
    }

    // Calculate phase voltages as sinusoids
    float Ua = sinf(open_loop_theta);
    float Ub = sinf(open_loop_theta - 2.0f * M_PI / 3.0f);
    float Uc = sinf(open_loop_theta + 2.0f * M_PI / 3.0f);

    // Convert to PWM duty based on open loop duty limit
    uint16_t dutyA = (uint16_t)((Ua + 1.0f) * 0.5f * open_loop_duty);
    uint16_t dutyB = (uint16_t)((Ub + 1.0f) * 0.5f * open_loop_duty);
    uint16_t dutyC = (uint16_t)((Uc + 1.0f) * 0.5f * open_loop_duty);

    // Set PWM duties
    motor->pwmU->setDuty(motor->pwmU, dutyA);
    motor->pwmV->setDuty(motor->pwmV, dutyB);
    motor->pwmW->setDuty(motor->pwmW, dutyC);
}

float kp_values[] = {0.01f, 0.02f, 0.03f, 0.04f, 0.05f, 0.06f,0.07f, 0.08f};
float ki_values[] = {0.005f, 0.01f,0.015f,0.02f, 0.03f};
int num_kp = sizeof(kp_values) / sizeof(kp_values[0]);
int num_ki = sizeof(ki_values) / sizeof(ki_values[0]);

void sweep_gains(MotorController_t *motor) {
    for (int i = 0; i < num_kp; i++) {
        for (int j = 0; j < num_ki; j++) {
            motor->kp_velocity = kp_values[i];
            motor->ki_velocity = ki_values[j];

            printf("Testing Kp=%.3f, Ki=%.3f\n", motor->kp_velocity, motor->ki_velocity);

            // Reset integral term
            motor->velocity_error_integral = 0.0f;

            // Set some test velocity setpoint, e.g. 50 deg/s
            motor->velocity_setpoint = 10.0f;

            // Run your motor update loop for some duration or iterations
            for (int step = 0; step < 10000; step++) {
                motor_update(motor);
                // Add delay here if necessary for timing, e.g. 10 ms
            }

            // Optionally, record results, check overshoot, steady state error, etc.
            // You might want to add a mechanism to capture these metrics.
        }
    }
}