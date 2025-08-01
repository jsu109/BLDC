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

typedef struct ENCODER_HAL {
    encoderId_t id;

    SPI_hal_t spiInst;
    SPI_data_t spiData;
    SPI_settings_t spiSettings;
    
    GPIO_hal_t cs_gpioInst;
    GPIO_settings_t cs_gpioSettings;

    void (*init)(encoderHal_t *encoder);
    void (*config)(encoderHal_t *encoder);
    uint16_t (*read)(encoderHal_t *encoder);
    float (*process)(uint16_t angle);
} encoderHal_t;

bool encoderHalInit(encoderHal_t *hal);
void encoderHalSetConfig(encoderHal_t *hal);
uint16_t encoderHalReadAngleMeasurement(encoderHal_t *hal);
float encoderHalProcessMeasurement(encoderHal_t *hal, uint16_t angle);

#endif  // encoder_HAL_H
