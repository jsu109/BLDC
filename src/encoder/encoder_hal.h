#ifndef encoder_HAL_H
#define encoder_HAL_H

#include <stdbool.h>
#include <stdint.h>
#include "spi_hal.h"
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

    encoderComm_t comm;
    
    GPIO_hal_t cs_gpioInst;
    GPIO_settings_t cs_gpioSettings;

    void (*init)(encoderHal_t *encoder);
    void (*config)(encoderHal_t *encoder);
    void (*read)(encoderHal_t *encoder);
    void (*process)(encoderHal_t *encoder);
} encoderHal_t;

bool encoderHalInit(encoderHal_t *hal);

#endif  // encoder_HAL_H
