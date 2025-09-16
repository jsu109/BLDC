#include "motor_control.h"
#include <math.h>
#include <stdio.h>
#include "pico/stdlib.h"
// --- Field-Oriented Control (Voltage-based) ---
// Uses velocity PI to generate Iq-like voltage command
// Id_ref = 0 (optimal flux), inverse Park -> alpha-beta -> three-phase -> PWM

static inline float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// Inverse Park: vd,vq -> v_alpha,v_beta
static inline void inv_park(float vd, float vq, float theta_rad, float *v_alpha, float *v_beta) {
    float c = cosf(theta_rad);
    float s = sinf(theta_rad);
    *v_alpha = vd * c - vq * s;
    *v_beta  = vd * s + vq * c;
}

// Alpha-beta -> three-phase (Clarke inverse)
static inline void alpha_beta_to_abc(float v_alpha, float v_beta, float *Va, float *Vb, float *Vc) {
    *Va = v_alpha;
    *Vb = -0.5f * v_alpha + 0.86602540378f * v_beta;
    *Vc = -0.5f * v_alpha - 0.86602540378f * v_beta;
}

// Voltage -> PWM duty (0..MAX_DUTY)
static inline uint16_t voltage_to_duty(float v_phase, float v_scale) {
    float half = ((float)MAX_DUTY) * 0.5f;
    float duty = (v_phase / v_scale) * half + half;
    duty = clampf(duty, 0.0f, (float)MAX_DUTY);
    return (uint16_t)(duty + 0.5f);
}

// Initialize motor controller
bool motor_init(MotorController_t *motor) {
    if (!motor || !motor->encoder || !motor->pwmU || !motor->pwmV || !motor->pwmW) return false;
    
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

    motor->measured_velocity_dps = 0.0f;
    motor->target_velocity_dps = 0.0f;
    motor->velocity_target_dps = 0.0f;
    motor->last_angle_deg = motor->encoder->angleDegrees;
    motor->theta_elec = 0.0f;
    motor->integral = 0.0f;
    motor->Iq_last = 0.0f;
    if (motor->Iq_max <= 0.0f) motor->Iq_max = (float)MAX_DUTY;
    return true;
}

void motor_set_max_duty(MotorController_t *motor, uint16_t duty) {
    if (motor) motor->Iq_max = (float)clampf((float)duty, 0.0f, (float)MAX_DUTY);
}

// Update measured velocity using encoder
void motor_update_velocity(MotorController_t *motor, float dt_s) {
    if (!motor || dt_s <= 0.0f) return;

    motor->encoder->read(motor->encoder);
    motor->encoder->process(motor->encoder);

    float angle = motor->encoder->angleDegrees;
    float delta = angle - motor->last_angle_deg;
    if (delta > 180.0f) delta -= 360.0f;
    if (delta < -180.0f) delta += 360.0f;

    float vel_raw = delta / dt_s; // deg/s
    motor->measured_velocity_dps = VEL_FILTER_ALPHA * vel_raw + (1.0f - VEL_FILTER_ALPHA) * motor->measured_velocity_dps;

    motor->last_angle_deg = angle;
}

// Velocity PI control + FOC
void motor_velocity_control(MotorController_t *motor, float target_velocity_dps, float dt_s) {
    if (!motor) return;
    
    motor_update_velocity(motor, dt_s);
    float vel_error = target_velocity_dps - motor->measured_velocity_dps;
    
    // PI integrator
    motor->integral += KI_VEL * vel_error * dt_s;
    motor->integral = clampf(motor->integral, -INTEGRAL_MAX, INTEGRAL_MAX);

    float Iq_ref = KP_VEL * vel_error + motor->integral;
    
    // Clamp and slew limit Iq
    float Iq_limit = motor->Iq_max;
    Iq_ref = clampf(Iq_ref, -Iq_limit, Iq_limit);
    float delta = Iq_ref - motor->Iq_last;
    float max_delta = Iq_SLEW_DPS * dt_s;
    if (delta > max_delta) delta = max_delta;
    if (delta < -max_delta) delta = -max_delta;
    motor->Iq_last += delta;
    Iq_ref = motor->Iq_last;
    
    
    // FOC: Id = 0, Vq = Iq_ref
    float Vd = 0.0f;
    float Vq = Iq_ref;

    // Electrical angle
    float mech_deg = motor->encoder->angleDegrees;
    float elec_deg = fmodf(-mech_deg * motor->pole_pairs + motor->elec_offset, 360.0f);
    if (elec_deg < 0.0f) elec_deg += 360.0f;
    motor->theta_elec = elec_deg;
    float theta_rad = elec_deg * (3.14159265358979323846f / 180.0f);
    
    // Inverse Park
    float v_alpha, v_beta;
    inv_park(Vd, Vq, theta_rad, &v_alpha, &v_beta);
   
    // Alpha-beta -> 3-phase
    float Va, Vb, Vc;
    alpha_beta_to_abc(v_alpha, v_beta, &Va, &Vb, &Vc);
     
    // Scale to Iq_max
    float maxAbs = fmaxf(fabsf(Va), fmaxf(fabsf(Vb), fabsf(Vc)));
    if (maxAbs > Iq_limit) {
        float scale = Iq_limit / maxAbs;
        Va *= scale; Vb *= scale; Vc *= scale;
    }
    

    // Map to PWM
    uint16_t dutyU = voltage_to_duty(Va, Iq_limit);
    uint16_t dutyV = voltage_to_duty(Vb, Iq_limit);
    uint16_t dutyW = voltage_to_duty(Vc, Iq_limit);
  

    if (dutyU < MIN_PHASE_DUTY) dutyU = MIN_PHASE_DUTY;
    if (dutyV < MIN_PHASE_DUTY) dutyV = MIN_PHASE_DUTY;
    if (dutyW < MIN_PHASE_DUTY) dutyW = MIN_PHASE_DUTY;

    motor->pwmU->setDuty(motor->pwmU, dutyU);
    motor->pwmV->setDuty(motor->pwmV, dutyV);
    motor->pwmW->setDuty(motor->pwmW, dutyW);

    // Debug
    printf("target: %.1f, vel: %.1f, err: %.1f, Iq: %.2f, theta: %.1f, angle:%.1f, duties U:%u V:%u W:%u\n",
           target_velocity_dps, motor->measured_velocity_dps, vel_error, Iq_ref,
           motor->theta_elec, motor->encoder->angleDegrees, dutyU, dutyV, dutyW);
}

// Sinusoidal commutation helper (for test or startup)
void commutate_sinusoidal(MotorController_t *motor, float elec_angle_deg, float Iq) {
    if (!motor) return;
    float theta_rad = elec_angle_deg * (3.14159265358979323846f / 180.0f);
    float Va = Iq * sinf(theta_rad);
    float Vb = Iq * sinf(theta_rad - 2.094395f); // -120 deg
    float Vc = Iq * sinf(theta_rad + 2.094395f); // +120 deg
    motor->pwmU->setDuty(motor->pwmU, voltage_to_duty(Va, Iq));
    motor->pwmV->setDuty(motor->pwmV, voltage_to_duty(Vb, Iq));
    motor->pwmW->setDuty(motor->pwmW, voltage_to_duty(Vc, Iq));
}

// Offset calibration (encoder alignment)
void motor_calibrate_offset(MotorController_t *motor, float test_duty)
{
    // Apply small voltage to spin the motor slowly in one direction
    const float step_deg = 1.0f;   // step increment in degrees
    const uint16_t steps = 720;    // full rotation

    for (uint16_t i = 0; i < steps; i++) {
        // Apply small voltage in open-loop
        commutate_sinusoidal(motor, (float)i, test_duty);
        // Small delay to allow motor to move
        sleep_ms(5);
    }

    // Read current encoder mechanical angle
    motor->encoder->read(motor->encoder);
    motor->encoder->process(motor->encoder);
    float mech_angle = motor->encoder->angleDegrees;
    if (motor->encoder_reversed) mech_angle = -mech_angle;

    // Calculate electrical offset
    motor->elec_offset = fmodf(mech_angle * motor->pole_pairs, 360.0f);

    printf("Motor calibration complete, elec_offset = %.2f deg\n", motor->elec_offset);
}

// Startup routine (smooth ramp)
void motor_startup(MotorController_t *motor, float target_velocity_dps) {
    if (!motor) return;
    float ramp_time_s = 2.0f;
    float dt_s = CONTROL_PERIOD_MS * 0.001f;
    int steps = (int)(ramp_time_s / dt_s);
    for (int i = 0; i <= steps; i++) {
        float vel = target_velocity_dps * ((float)i / steps);
        motor_velocity_control(motor, vel, dt_s);
    }
}

// Generic update (alias for velocity control)
void motor_update(MotorController_t *motor) {
    if (!motor) return;
    motor_velocity_control(motor, motor->target_velocity_dps, CONTROL_PERIOD_MS * 0.001f);
}