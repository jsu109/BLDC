#include "spi_hal.h"
#include "rp2040_spi.h"
void spi_hal_init(SPI_hal_t *spiInstance, SPI_settings_t *spiSettings)
{
    switch(spiSettings->sysType) {
        case RP2040:
            spiInstance->init = rp2040_spi_init;
            break;
        default:
            break;
    }

    if(spiInstance->init) {
        spiInstance->init(spiSettings);
    }
} 


void spi_hal_config(SPI_hal_t *spiInstance, SPI_settings_t *spiSettings)
{
    switch(spiSettings->sysType) {
        case RP2040:
            spiInstance->setFormat = rp2040_spi_setFormat;
            break;
        default:
            break;
    }

    if(spiInstance->setFormat) {
        spiInstance->setFormat(spiSettings);
    }
}

// Transfer 16 bits of data via SPI, calls platform-specific transfer16 function pointer
uint16_t spi_hal_transfer16(SPI_hal_t *spiInstance, SPI_settings_t *spiSettings, uint16_t command, uint16_t *result)
{
    switch(spiSettings->sysType) {
        case RP2040:
            spiInstance->transfer16 = rp2040_spi_transfer16;
            break;
        default:
            break;
    }

    if(spiInstance->transfer16) {
        return spiInstance->transfer16(command,result,spiSettings);
    }
}