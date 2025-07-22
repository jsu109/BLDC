#include "stdint.h"
#include "AS5048A.h"
#include "spi_hal.h"

typedef struct {
    
} AS5048A_t;

static AS5048A_t AS5048A;

 void AS5048AInit(void)
{
    // Initialization for AS5048A
    SPI_settings_t AS5048A_spiSettings = {
    .sysType = RP2040,
    .baudrate = 80000,
    .dataBits = AS5048A_DATA_BITS,
    .order = AS5048A_BITORDER,
    .cpol = 0,
    .cpha = 0,
    .hw_handle = spi0
    };
    SPI_hal_t AS5048A_spiInst;
    spi_hal_init(&AS5048A_spiInst, &AS5048A_spiSettings);
    spi_hal_config(&AS5048A_spiInst,&AS5048A_spiSettings);
}

 void AS5048ASetConfigReg(void) 
{
    // configure settings in AS5048A
   
}

 void AS5048AReadAngleMeasurement(void) 
{
   
    
    
}

 float AS5048AProcessAngleMeasurement(void) 
{
    // Implementation of the function
    
}
