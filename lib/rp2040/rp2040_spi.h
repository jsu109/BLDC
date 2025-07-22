#ifndef RP2040_SPI_H
#define RP2040_SPI_H

#include <stdint.h>
#include "spi_hal.h"

void rp2040_spi_init( SPI_settings_t *settings);


void rp2040_spi_setFormat( SPI_settings_t *settings);


uint16_t rp2040_spi_transfer16(uint16_t command, uint16_t *result, SPI_settings_t *settings);


#endif 