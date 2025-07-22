#ifndef GPIO_HAL_H
#define GPIO_HAL_H

#include <stdint.h>
#include <stdbool.h>


typedef enum {
    RP2040,
    STM32
} sysType_t;
typedef struct {
    uint8_t gpioPin;
    bool out;
    sysType_t sysType;
    void *gpioFunction;
} GPIO_settings_t;

typedef struct {
    GPIO_settings_t settings;

    void (*init)(GPIO_settings_t *);
    void (*put)(GPIO_settings_t *, bool);
} GPIO_hal_t;

void gpio_hal_init(GPIO_hal_t *gpioInstance, GPIO_settings_t *gpioSettings);
void gpio_hal_put(GPIO_hal_t *gpioInstance, GPIO_settings_t *gpioSettings, bool val);


#endif