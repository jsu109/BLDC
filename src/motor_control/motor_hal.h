#ifndef MOTOR_HAL_H
#define MOTOR_HAL_H
// Planning
/*This is where things like specific hardware is called to implement motor control
*/
typedef struct
{
    uint8_t GPIO;
    uint8_t sliceNum;

} phase_t;


    // Initialize PWM for a given motor phase.
    void motor_hal_pwm_init(uint8_t phase);

    // Set duty cycle for a motor phase in the range [0.0, 1.0].
    void motor_hal_set_pwm(uint8_t phase, float duty_cycle);
    
    // Enable PWM output for a phase.
    void motor_hal_pwm_enable(uint8_t phase);

    // Disable PWM output for a phase.
    void motor_hal_pwm_disable(uint8_t phase);
    


#endif //MOTOR_HAL_H