#include "rp2040_spi.h"
#include "hardware/spi.h"
#include <stdio.h>
static inline spi_inst_t *get_spi_instance(uint8_t hw_handle) {
    switch (hw_handle) {
        case 0:
            return spi0;
        case 1:
            return spi1;
        default:
            return NULL; // Or handle error
    }
}

void rp2040_spi_init(SPI_settings_t *settings) {
    spi_inst_t *spi_instance = get_spi_instance(settings->hw_handle);
    // spi_init(spi_instance, settings->baudrate);
    spi_init(spi0, 1000000); // Set SPI baud rate to 1 MHz

    

}
    

void rp2040_spi_setFormat(SPI_settings_t *settings) {
    spi_inst_t *spi_instance = get_spi_instance(settings->hw_handle);
    spi_set_format(spi_instance, settings->dataBits, settings->cpol, settings->cpha, SPI_MSB_FIRST);
}

uint16_t rp2040_spi_transfer16(SPI_hal_t *spiInstance) {
    spi_inst_t *rp2040Instance = get_spi_instance(spiInstance->settings.hw_handle);
    switch (spiInstance->data->RW) {
        case 0: {
            spi_write16_read16_blocking(rp2040Instance, &spiInstance->data->cmd, &spiInstance->data->res, spiInstance->data->len);
            
            return 0;
        }
        case 1:
            spi_write16_blocking(rp2040Instance, &spiInstance->data->cmd, spiInstance->data->len);
            return 0;
    }
    return 0;
}