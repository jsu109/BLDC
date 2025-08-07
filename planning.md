# Motor Control System Plan

## 🎯 Goals

- Modularize motor control logic (`motor_controller`)
- Structure ADC and Timer interactions via HALs
- Improve scheduling and timing without full RTOS (yet)
- Maintain clean hardware abstraction and scalability

---

## 🧱 Module Overview

### 1. `motor_controller`
Encapsulate BLDC commutation and control logic.

- Modes: Open-loop, Encoder-closed-loop, Disabled
- Functions:
  - `motor_init()`
  - `motor_update()`
  - `motor_setMode()`, `motor_setDuty()`, `motor_setAngle()`, `motor_stop()`

### 2. `adc_hal`
Abstract ADC readings (e.g. pot voltage).

- Functions:
  - `adc_hal_init()`
  - `adc_hal_readRaw()`
  - `adc_hal_readVoltage()`

### 3. `timer_hal`
Provide consistent timebase for scheduling.

- Functions:
  - `timer_hal_getTimeUs()`
  - `timer_hal_elapsedMs()`
  - `timer_hal_delayMs()`, `timer_hal_delayUs()`

---

## 🧠 Scheduling Strategy (No RTOS)

Use cooperative time slicing:

```c
const uint32_t LOOP_PERIOD_US = 1000;
uint64_t last_time = timer_hal_getTimeUs();

while (true) {
    if (timer_hal_getTimeUs() - last_time >= LOOP_PERIOD_US) {
        last_time += LOOP_PERIOD_US;

        // 1ms tick functions:
        read inputs (ADC, encoders)
        motor_controller_update()
        handle control state
    }
}
