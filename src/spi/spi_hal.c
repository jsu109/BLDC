#include "spi_hal.h"
#include "rp2040_spi.h"
void spi_hal_init(sysType_t sysType, SPI_hal_t *spiInstance)
{
    switch(sysType) {
        case RP2040:
            spiInstance->init = rp2040_spi_init;
}
} 


void spi_hal_config(SPI_hal_t *spiInstance, SPI_settings_t *spiSettings)
{
    if (spiInstance && spiInstance->setFormat && spiSettings)
        {
            spiInstance->settings = *spiSettings;  // Copy settings to instance
            spiInstance->setFormat(spiSettings);
        }
}

// Transfer 16 bits of data via SPI, calls platform-specific transfer16 function pointer
uint16_t spi_hal_transfer16(SPI_hal_t *spiInstance, uint16_t data)
{
    if (spiInstance && spiInstance->transfer16)
    {
        return spiInstance->transfer16(data);
    }
    return 0; // or some error indicator
}