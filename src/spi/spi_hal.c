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


void spi_hal_config(SPI_hal_t *spiInstance)
{

    if(spiInstance->config) {
        spiInstance->config(&spiInstance->settings);
    }
}

// Transfer 16 bits of data via SPI, calls platform-specific transfer16 function pointer
uint16_t spi_hal_transfer16(SPI_hal_t *spiInstance)
{
    if(spiInstance->transfer16) {
        
        return spiInstance->transfer16(&spiInstance->settings,&spiInstance->data);
    }
}
