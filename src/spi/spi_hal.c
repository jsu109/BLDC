#include "spi_hal.h"
#include "rp2040_spi.h"
#include <stdio.h>
void spi_hal_init(SPI_hal_t *spiInstance, SPI_settings_t *spiSettings, SPI_data_t *spiData)
{
    spiInstance->data = *spiData;
    spiInstance->settings = *spiSettings;
    switch(spiSettings->sysType) {
        case RP2040:
            spiInstance->init = rp2040_spi_init;
            spiInstance->config = rp2040_spi_setFormat;
            spiInstance->transfer16 = rp2040_spi_transfer16;
            break;
        default:
            break;
    }

    if(spiInstance->init) {
        spiInstance->init(spiSettings);
    }
} 

