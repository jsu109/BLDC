#ifndef SPI_HAL_H
#define SPI_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include "sysType.h"
typedef enum {
    SPI_HAL_MSB_FIRST,
    SPI_HAL_LSB_FIRST
} spi_hal_order_t;

typedef enum {
    SPI_HAL_CPOL_0,
    SPI_HAL_CPOL_1
} spi_hal_cpol_t;

typedef enum {
    SPI_HAL_CPHA_0,
    SPI_HAL_CPHA_1
} spi_hal_cpha_t;

typedef struct {
    int32_t baudrate;
    uint8_t dataBits;
    spi_hal_order_t order;
    spi_hal_cpol_t cpol;
    spi_hal_cpha_t cpha;
    sysType_t sysType;
    uint8_t hw_handle; // e.g. pointer to spi0 or spi1
} SPI_settings_t;

typedef struct {
    uint16_t cmd;
    uint16_t res;
    uint8_t len;
    bool RW;

}
SPI_data_t;
typedef struct SPI_hal SPI_hal_t;

typedef struct SPI_hal {
    SPI_settings_t settings;
    SPI_data_t data;
    void (*init)(SPI_settings_t *);
    void (*config)(SPI_settings_t *);
    uint16_t (*transfer16)(SPI_hal_t *spiInstance);
} SPI_hal_t;

void spi_hal_init(SPI_hal_t *spiInstance, SPI_settings_t *spiSettings, SPI_data_t *spiData);

void spi_hal_updateData(SPI_hal_t *spiInstance, uint16_t cmd, uint16_t res,uint8_t len,bool RW);


#endif