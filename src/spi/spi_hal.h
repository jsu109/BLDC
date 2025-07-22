#ifndef SPI_HAL_H
#define SPI_HAL_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    DEFAULT,
    RP2040,
    // Add other sensor IDs here
} sysType_t;
typedef struct 
{
    int16_t baudrate;
    int8_t dataBits;
    bool dataOrder;


} SPI_settings_t;
typedef struct 
{
    SPI_settings_t settings;
    void(*init)(void);
    void(*setFormat)(void*);
    uint16_t(*transfer16)(uint16_t);

} SPI_hal_t;



void spi_hal_init(sysType_t sysType, SPI_hal_t *spiInstance);

void spi_hal_config(SPI_hal_t *spiInstance, SPI_settings_t *spiSettings);

uint8_t spi_hal_transfer8(SPI_hal_t *spiInstance, uint8_t data);

#endif