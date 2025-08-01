#include "encoder_hal.h"
#include "gpio_hal.h"
#include "spi_hal.h"
#include "AS5048A.h"
#include "pico/stdlib.h"
#include "stddef.h"



bool encoderHalInit(encoderHal_t *hal) 
{
    
    if (hal->id != NULL) {
        
        switch (hal->id)
        {
        case encoder_ID_AS5048A:
            hal->init = AS5048AInit;
            hal->config = AS5048ASetConfigReg;
            hal->read = AS5048AReadAngle;
            hal->process = AS5048AProcessAngleMeasurement;
            hal->zero = AS5048AZero;
            
            break;
        
        default:
            // Handle unsupported sensor ID if necessary
            hal->init = NULL;
            hal->config = NULL;
            hal->read = NULL;
            hal->process = NULL;
            break;
        }
        // hal->init(hal);
        //initalise CS pin
        gpio_hal_init(&hal->cs_gpioInst,&hal->cs_gpioSettings);
        
        //initalise SPI for encoder
        spi_hal_init(&hal->comm.spi.spiInst,&hal->comm.spi.spiSettings,&hal->comm.spi.spiData,&hal->cs_gpioInst);
        hal->comm.spi.spiInst.config(&hal->comm.spi.spiSettings);
        hal->init(hal);
        return 0;

    } 
    else {
        return 1;
    }
}


