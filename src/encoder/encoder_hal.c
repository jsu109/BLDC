#include "encoder_hal.h"
#include "AS5048A.h"
#include "pico/stdlib.h"
#include "stddef.h"



bool encoderHalInit(encoderId_t id, encoderHal_t *hal) 
{
    
    if (hal != NULL) {
    
        switch (id)
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
        
        hal->init();
        
        return 0;

    } 
    else {
        return 1;
    }
}

void encoderHalSetConfig(encoderHal_t *hal)
{
    hal->config();
}
uint16_t encoderHalReadAngleMeasurement(encoderHal_t *hal)
{
    return hal->read();
}
float encoderHalProcessMeasurement(encoderHal_t *hal, uint16_t angle)
{
   return hal->process(angle);
}

