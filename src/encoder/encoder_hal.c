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
        spi_hal_init(&hal->spiInst,&hal->spiSettings,&hal->spiData,&hal->cs_gpioInst);
        hal->spiInst.config(&hal->spiSettings);
        hal->init(hal);
        return 0;

    } 
    else {
        return 1;
    }
}

void encoderHalSetConfig(encoderHal_t *hal)
{
    // 
    // hal->config(hal);
}

uint16_t encoderHalReadAngleMeasurement(encoderHal_t *hal)
{
    return hal->read(hal);
}
float encoderHalProcessMeasurement(encoderHal_t *hal, uint16_t angle)
{
   return hal->process(angle);
}

