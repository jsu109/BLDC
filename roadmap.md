flowchart TD

    subgraph BaseHW[BLDC Test Platform - Hardware Layer]
        A1[DRV8317 Driver] --> A2[BLDC Motor]
        A1 --> A3[Encoders (AS5048A)]
        A1 --> A4[ADC Inputs (Pot/Sensors)]
        A1 --> A5[GPIO / Fault Signals]
        A1 --> A6[RP2040 MCU]
    end

    subgraph HAL[Hardware Abstraction Layer]
        H1[gpio_hal] --> MC
        H2[pwm_hal] --> MC
        H3[encoder_hal] --> MC
        H4[adc_hal] --> MC
        H5[timer_hal] --> MC
    end

    subgraph MC[Motor Control Core]
        M1[Open-loop Commutation]
        M2[Encoder-based Commutation]
        M3[Speed Control (PI)]
        M4[Position Control (PID)]
        M5[Fault Handling]
    end

    subgraph SCHED[Scheduler & System Layer]
        S1[Cooperative Loop]
        S2[Time-sliced Tasks]
        S3[FreeRTOS (optional)]
    end

    subgraph APP[Application Layer]
        AP1[Lab Mode: ADC -> Duty]
        AP2[Speed Mode: Command RPM]
        AP3[Position Mode: Command Angle]
        AP4[Waveform Mode: Sine/Trap Test]
        AP5[Integration: CNC Axis / Robot Joint / Gimbal]
    end

    BaseHW --> HAL
    HAL --> MC
    MC --> SCHED
    SCHED --> APP
