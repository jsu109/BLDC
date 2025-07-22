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

uint16_t rp2040_spi_transfer16(uint16_t data, SPI_settings_t *settings) {
    spi_inst_t *spi = (spi_inst_t *)settings->hw_handle;
    uint8_t buf[2] = {data >> 8, data & 0xFF};
    uint8_t rx[2];
    spi_write_read_blocking(spi, buf, rx, 2);
    return (rx[0] << 8) | rx[1];
}