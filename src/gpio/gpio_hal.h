#ifndef GPIO_HAL_H
#define GPIO_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include "sysType.h"

// gpio_hal.h
typedef enum { 
    GPIO_HAL_FUNC_XIP = 0, 
    GPIO_HAL_FUNC_SPI = 1,
    GPIO_HAL_FUNC_UART = 2,
    GPIO_HAL_FUNC_I2C = 3,
    GPIO_HAL_FUNC_PWM = 4,
    GPIO_HAL_FUNC_SIO = 5,
    GPIO_HAL_FUNC_PIO0 = 6,
    GPIO_HAL_FUNC_PIO1 = 7,
    GPIO_HAL_FUNC_GPCK = 8,
    GPIO_HAL_FUNC_USB = 9,
    GPIO_HAL_FUNC_NULL = 0x1f
} gpio_hal_function_t;



typedef struct {
    uint8_t gpioPin;
    bool out;
    sysType_t sysType;
    int gpioFunction;
} GPIO_settings_t;

typedef struct {
    GPIO_settings_t settings;

    void (*init)(GPIO_settings_t *);
    void (*put)(GPIO_settings_t *, bool);
} GPIO_hal_t;

void gpio_hal_init(GPIO_hal_t *gpioInstance, GPIO_settings_t *gpioSettings);
void gpio_hal_put(GPIO_hal_t *gpioInstance, GPIO_settings_t *gpioSettings, bool val);


#endif