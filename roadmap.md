

```mermaid

    graph TD;

    subgraph BaseHW["BLDC Test Platform - Hardware Layer"]
        A1[DRV8317 Driver];
        A2[BLDC Motor];
        A3[Encoders (AS5048A)];
        A4[ADC Inputs (Pot/Sensors)];
        A5[GPIO / Fault Signals];
        A6[RP2040 MCU];

        A1 --> A2;
        A1 --> A3;
        A1 --> A4;
        A1 --> A5;
        A1 --> A6;
    end

    subgraph MC["Motor Control Core"]
        M1[Open-loop Commutation];
        M2[Encoder-based Commutation];
        M3[Speed Control (PI)];
        M4[Position Control (PID)];
        M5[Fault Handling];
    end

    subgraph HAL["Hardware Abstraction Layer"]
        H1[gpio_hal];
        H2[pwm_hal];
        H3[encoder_hal];
        H4[adc_hal];
        H5[timer_hal];

        H1 --> M1;
        H2 --> M1;
        H3 --> M1;
        H4 --> M1;
        H5 --> M1;
    end

    subgraph SCHED["Scheduler & System Layer"]
        S1[Cooperative Loop];
        S2[Time-sliced Tasks];
        S3[FreeRTOS (optional)];
    end

    subgraph APP["Application Layer"]
        AP1[Lab Mode: ADC -> Duty];
        AP2[Speed Mode: Command RPM];
        AP3[Position Mode: Command Angle];
        AP4[Waveform Mode: Sine/Trap Test];
        AP5[Integration: CNC Axis / Robot Joint / Gimbal];
    end

    %% Connections between layers by nodes (typically MC or SCHED nodes)
    A6 --> H1;
    A6 --> H2;
    A6 --> H3;
    A6 --> H4;
    A6 --> H5;

    M1 --> S1;
    M2 --> S1;
    M3 --> S1;
    M4 --> S1;
    M5 --> S1;

    S1 --> AP1;
    S2 --> AP2;
    S3 --> AP3;

```