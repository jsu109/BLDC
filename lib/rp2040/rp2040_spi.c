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

uint16_t rp2040_spi_transfer16(SPI_settings_t *settings, SPI_data_t *spiData) {
    spi_inst_t *spi = (spi_inst_t *)settings->hw_handle;
    switch (spiData->RW) {
        case 0:
            return spi_read16_blocking(spi,spiData->cmd, &spiData->res, spiData->len);
        case 1:
            spi_write16_blocking(spi,&spiData->cmd, spiData->len);
            return 0;
    }
    
}