#include "spi_hal.h"
#include "rp2040_spi.h"
#include <stdio.h>
void spi_hal_init(SPI_hal_t *spiInstance, SPI_settings_t *spiSettings, SPI_data_t *spiData, GPIO_hal_t *CS)
{
    spiInstance->data = spiData;
    spiInstance->settings = *spiSettings;
    spiInstance->CS = *CS;
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

void spi_hal_updateData(SPI_data_t *spiData, uint16_t cmd, uint16_t res,uint8_t len,bool RW) {
    spiData->cmd = cmd;
    spiData->res = res;
    spiData->len = len;
    spiData->RW = RW;
}