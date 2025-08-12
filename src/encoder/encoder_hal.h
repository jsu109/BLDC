#ifndef encoder_HAL_H
#define encoder_HAL_H

#include <stdbool.h>
#include <stdint.h>
#include "spi_hal.h"
#include "timer_hal.h"

typedef enum
{
    encoder_ID_UNKNOWN,
    encoder_ID_AS5048A,
    // Add other sensor IDs here
} encoderId_t;

typedef struct ENCODER_HAL encoderHal_t;

typedef struct {
    SPI_hal_t spiInst;
    SPI_data_t spiData;
    SPI_settings_t spiSettings;
} encoderSpi_t;

typedef struct {
    // I2C_hal_t i2cInst;
    // I2C_data_t i2cData;
    // I2C_settings_t i2cSettings;
} encoderI2c_t;

typedef union {
    encoderSpi_t spi;
    encoderI2c_t i2c;
} encoderComm_t;

typedef struct ENCODER_HAL {
    encoderId_t id;
    uint16_t rawAngle;
    float angleDegrees;
    uint32_t lastTimestampMs;   // Time of last reading
    uint32_t deltaTimeMs;       // Time between last two readings

    float velocityDegPerSec;
    float velocityRPM;
    float lastAngleDegrees;

    encoderComm_t comm;
    GPIO_hal_t cs_gpioInst;
    GPIO_settings_t cs_gpioSettings;
    timer_hal_t timer;

    void (*init)(encoderHal_t *encoder);
    void (*config)(encoderHal_t *encoder);
    void (*read)(encoderHal_t *encoder);
    void (*zero)(encoderHal_t *encoder1);
    void (*process)(encoderHal_t *encoder);
} encoderHal_t;

bool encoderHalInit(encoderHal_t *encoder);
void encoderHal_updateTimestamp(encoderHal_t *encoder);
void encoderHal_updateVelocity(encoderHal_t *encoder);

#endif  // encoder_HAL_H
