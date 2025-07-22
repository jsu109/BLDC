#include "stdint.h"
#include "AS5048A.h"
#include "spi_hal.h"
#include "gpio_hal.h"

typedef struct {
    
} AS5048A_t;

static AS5048A_t AS5048A;

 void AS5048AInit(void)
{
    // Initialization for AS5048A

    //define settings for SPI 
    SPI_settings_t AS5048A_spiSettings = {
    .sysType = RP2040,
    .baudrate = 80000,
    .dataBits = AS5048A_DATA_BITS,
    .order = AS5048A_BITORDER,
    .cpol = 0,
    .cpha = 0,
    .hw_handle = AS5048A_SPI_INSTANCE
    };

    //define GPIO settings for gpio pins being used
    GPIO_settings_t AS5048A_SCK_settings = {
        .sysType = RP2040,
        .gpioPin = AS5048A_SCK,
        .out = 0,
        .gpioFunction = GPIO_FUNC_SPI,
    };
    GPIO_settings_t AS5048A_TX_settings = {
        .sysType = RP2040,
        .gpioPin = AS5048A_TX,
        .out = 0,
        .gpioFunction = GPIO_FUNC_SPI,
    };
    GPIO_settings_t AS5048A_RX_settings = {
        .sysType = RP2040,
        .gpioPin = AS5048A_RX,
        .out = 0,
        .gpioFunction = GPIO_FUNC_SPI,
    };
    GPIO_settings_t AS5048A_CS_settings = {
        .sysType = RP2040,
        .gpioPin = AS5048A_CS,
        .out = 0,
        .gpioFunction = GPIO_FUNC_SPI,
    };
    

    // declare spi and gpio instances used for AS5048A
    SPI_hal_t AS5048A_spiInst;  //spi0 -> 
    GPIO_hal_t AS5048A_SCK_gpioInst; //SCK pin
    GPIO_hal_t AS5048A_TX_gpioInst; //TX pin
    GPIO_hal_t AS5048A_RX_gpioInst; //RX pin
    GPIO_hal_t AS5048A_CS_gpioInst; //CS pin

    //initalise GPIO being used for AS5048A
    gpio_hal_init(&AS5048A_SCK_gpioInst,&AS5048A_SCK_settings);
    gpio_hal_init(&AS5048A_TX_gpioInst,&AS5048A_TX_settings);
    gpio_hal_init(&AS5048A_RX_gpioInst,&AS5048A_RX_settings);
    gpio_hal_init(&AS5048A_CS_gpioInst,&AS5048A_CS_settings);

    //initalise SPI for AS5048A
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
