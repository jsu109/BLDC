#include "encoder_hal.h"
#include "gpio_hal.h"
#include "spi_hal.h"
#include "AS5048A.h"
#include "pico/stdlib.h"
#include "stddef.h"
#include <stdio.h>


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
        timer_hal_init(hal->timer);
        hal->comm.spi.spiInst.config(&hal->comm.spi.spiSettings);
        hal->init(hal);
        return 1;

    } 
    else {
        return 0;
    }
}


void encoderHal_updateTimestamp(encoderHal_t *encoder)
{
    if (encoder == NULL || encoder->timer == NULL) {
        return; // Safety check
    }

    uint32_t now = encoder->timer->get_ms(encoder->timer);

    // Calculate delta time (handles wraparound automatically for unsigned types)
    encoder->deltaTimeMs = now - encoder->lastTimestampMs;

    // Store the new timestamp
    encoder->lastTimestampMs = now;
}

void encoderHal_updateVelocity(encoderHal_t *encoder)
{

    if (encoder == NULL || encoder->deltaTimeMs == 0) {
        return; // Safety check
    }

    // Calculate change in angle
    float deltaAngle = encoder->angleDegrees - encoder->lastAngleDegrees;

    // Handle wrap-around for angles (e.g., jumping from 359 to 0 degrees)
    if (deltaAngle > 180.0f) {
        deltaAngle -= 360.0f;
    } else if (deltaAngle < -180.0f) {
        deltaAngle += 360.0f;
    }

    // Convert delta angle and delta time to velocity
    float velocityDegPerSec = deltaAngle / ((float)encoder->deltaTimeMs / 1000.0f);
    float velocityRPM = (velocityDegPerSec / 360.0f) * 60.0f;

    // Store results
    encoder->velocityDegPerSec = velocityDegPerSec;
    encoder->velocityRPM = velocityRPM;

    // Update last angle for next calculation
    encoder->lastAngleDegrees = encoder->angleDegrees;
}
