#include "rp2040_spi.h"
#include "hardware/spi.h"
void rp2040_spi_init(SPI_settings_t *settings) {
    spi_inst_t *spi = (spi_inst_t *)settings->hw_handle;
    spi_init(spi, settings->baudrate);
}

void rp2040_spi_setFormat(SPI_settings_t *settings) {
    spi_inst_t *spi = (spi_inst_t *)settings->hw_handle;
    spi_set_format(spi, settings->dataBits, settings->cpol, settings->cpha, SPI_MSB_FIRST);
}

uint16_t rp2040_spi_transfer16(uint16_t command, uint16_t *result, SPI_settings_t *settings) {
    spi_inst_t *spi = (spi_inst_t *)settings->hw_handle;
    return spi_read16_blocking(spi,command, result ,settings->dataBits);
}