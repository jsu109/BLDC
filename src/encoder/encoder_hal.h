#ifndef encoder_HAL_H
#define encoder_HAL_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    encoder_ID_UNKNOWN,
    encoder_ID_AS5048A,
    // Add other sensor IDs here
} encoderId_t;

typedef struct
{
    void (*init)(void);
    void (*config)(void);
    void (*read)(void);
    float (*process)(void);
} encoderHal_t;

bool encoderHalInit(encoderId_t id, encoderHal_t *hal);
void encoderHalSetConfig(encoderHal_t *hal);
void encoderHalReadAngleMeasurement(encoderHal_t *hal);
float encoderHalProcessMeasurement(encoderHal_t *hal);

#endif  // encoder_HAL_H
