#ifndef MOTOR_HAL_H
#define MOTOR_HAL_H

#include <stdint.h>
// Planning
/*This is where things like specific hardware is called to implement motor control
*/
typedef struct
{
    uint8_t GPIO;
    uint8_t sliceNum;
    uint8_t chan;
    uint8_t wrap;

} phase_t;

    phase_t motor_hal_init_phase(uint8_t gpio);

    // Initialize PWM for a given motor phase.
    void motor_hal_pwm_init(phase_t phase);

    // Set duty cycle for a motor phase in the range [0.0, 1.0].
    void motor_hal_set_pwm(phase_t phase, float duty_cycle);
    
    // Enable PWM output for a phase.
    void motor_hal_pwm_enable(phase_t phase);

    // Disable PWM output for a phase.
    void motor_hal_pwm_disable(phase_t phase);
    


#endif //MOTOR_HAL_H