#include "stdint.h"
#include "AS5048A.h"
#include "spi_hal.h"
#include "encoder_hal.h"
#include "gpio_hal.h"
#include <stddef.h>
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <pico/stdlib.h>
typedef struct {
    
} AS5048A_t;

static AS5048A_t AS5048A;


//define GPIO settings for gpio pins being used
GPIO_settings_t AS5048A_SCK_settings = {
    .sysType = RP2040,
    .gpioPin = AS5048A_SCK,
    .out = 1,
    .gpioFunction = GPIO_HAL_FUNC_SPI,
};
GPIO_settings_t AS5048A_TX_settings = {
    .sysType = RP2040,
    .gpioPin = AS5048A_TX,
    .out = 1,
    .gpioFunction = GPIO_HAL_FUNC_SPI,
};
GPIO_settings_t AS5048A_RX_settings = {
    .sysType = RP2040,
    .gpioPin = AS5048A_RX,
    .out = 0,
    .gpioFunction = GPIO_HAL_FUNC_SPI,
};



GPIO_hal_t AS5048A_SCK_gpioInst; //SCK pin
GPIO_hal_t AS5048A_TX_gpioInst; //TX pin
GPIO_hal_t AS5048A_RX_gpioInst; //RX pin

//need a have a function to assign a CS pin. 
 void AS5048AInit(encoderHal_t *encoder)
{
    //initalise GPIO being used for AS5048A
    gpio_hal_init(&AS5048A_SCK_gpioInst,&AS5048A_SCK_settings);
    gpio_hal_init(&AS5048A_TX_gpioInst,&AS5048A_TX_settings);
    gpio_hal_init(&AS5048A_RX_gpioInst,&AS5048A_RX_settings);
    
    //set CS Pin
    encoder->cs_gpioInst.put(&encoder->cs_gpioInst,1);
    
    gpio_set_function(16,GPIO_FUNC_SPI);
    gpio_set_function(18,GPIO_FUNC_SPI);
    gpio_set_function(19,GPIO_FUNC_SPI);

    
    
}
// Calculate even parity and set MSB if needed
uint16_t build_command(uint16_t address) {
    address &= 0x7FFF;  // 15 bits, MSB parity not included here
    uint16_t count = 0;
    for (int i = 0; i < 15; i++) {
        if (address & (1 << i)) count++;
    }
    // EVEN parity bit at bit 15 (MSB)
    if (count % 2) {
        address |= (1 << 15);  // set MSB for odd count (make parity even)
    }
    return address;
}
// Clear error flag command - sends 1 time to the sensor
void AS5048AClearErrorFlag(encoderHal_t *encoder) {
    
    const uint16_t CLEAR_ERROR_FLAG_ADDR = 0x0001;
    uint16_t clear_command = AS5048A_RW | CLEAR_ERROR_FLAG_ADDR; // 0x4001
    // Calculate parity bit and add to MSB
    clear_command = build_command(clear_command);
    spi_hal_updateData(&encoder->spiData,clear_command,0,1,READ);
    encoder->cs_gpioInst.put(&encoder->cs_gpioInst, 0);
    encoder->spiInst.transfer16(&encoder->spiInst);
    encoder->cs_gpioInst.put(&encoder->cs_gpioInst, 1);

    // Send dummy read to receive error status response
    uint16_t nop = build_command(AS5048A_NOP);
    uint16_t response = 0;
    spi_hal_updateData(&encoder->spiData,nop,response,1,READ);
    encoder->cs_gpioInst.put(&encoder->cs_gpioInst, 0);
    encoder->spiInst.transfer16(&encoder->spiInst);
    encoder->cs_gpioInst.put(&encoder->cs_gpioInst, 1);

}
 void AS5048ASetConfigReg(encoderHal_t *encoder) 
{
    AS5048AClearErrorFlag(encoder);
 
    
}

// Your existing read angle function adapted for parity calculation and proper SPI calls

uint16_t AS5048AReadAngle(encoderHal_t *encoder) {
    
    
    uint16_t command = AS5048A_ANGLE_REG | AS5048A_RW;
    uint16_t tx = build_command(command);
    uint16_t rx = 0;
    spi_hal_updateData(&encoder->spiData,tx,rx,1,READ);
    encoder->cs_gpioInst.put(&encoder->cs_gpioInst, 0); // CS low
    encoder->spiInst.transfer16(&encoder->spiInst); //Write
    encoder->cs_gpioInst.put(&encoder->cs_gpioInst, 1); // CS high
    uint16_t nop = build_command(AS5048A_NOP);  // Applies parity
    spi_hal_updateData(&encoder->spiData,nop,0,1,READ);
    sleep_us(1);
    encoder->cs_gpioInst.put(&encoder->cs_gpioInst, 0); // CS low
    encoder->spiInst.transfer16(&encoder->spiInst); //read 
    encoder->cs_gpioInst.put(&encoder->cs_gpioInst, 1);
    

    rx = encoder->spiData.res;
    
    
    
    return rx & AS5048A_RESULT_MASK;
}

float AS5048AProcessAngleMeasurement (uint16_t angle) 
{
    return (angle * 360.0f)/16384.0f;
}


// void setZero()
// {
//     // Step 1: Construct commands for ZPOSM and ZPOSL
//     uint16_t zposm_command = AS5048A_OTPHIGH_REG | !AS5048A_RW;  // Write command for MSB
//     zposm_command |= calculate_even_parity(zposm_command);           // Add parity bit

//     uint16_t zposl_command = AS5048A_OTPLOW_REG | !AS5048A_RW;  // Write command for LSB
//     zposl_command |= calculate_even_parity(zposl_command);           // Add parity bit

//     // Step 2: Write 0x0000 to OTP registers to clear previous zero position
//     gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,0);  // Assert chip select (active low)
    
//     spi_hal_updateDataAndTransfer(&AS5048A_spiInst,zposm_command,1,WRITE); 
    
//     spi_hal_updateDataAndTransfer(&AS5048A_spiInst,0x0000,1,WRITE); //clear MSB 

//     gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,1);  // Deassert chip select

//     gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,0);  // Assert chip select again
//     spi_hal_updateDataAndTransfer(&AS5048A_spiInst,zposl_command,1,WRITE); //clear MSB
//     spi_hal_updateDataAndTransfer(&AS5048A_spiInst,0x0000,1,WRITE);  // Clear LSB
//     gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,1);  // Deassert chip select

//     // Step 3: Read the current angle from the sensor
//     uint16_t current_angle = AS5048AReadAngle();

//     // Step 4: Write the current angle to OTP registers to set new zero position
//     gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,0);  // Assert chip select (active low)
//     spi_hal_updateDataAndTransfer(&AS5048A_spiInst,zposm_command,1,WRITE); 
    
//     spi_hal_updateDataAndTransfer(&AS5048A_spiInst,current_angle,1,WRITE);  

//     gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,1);  // Deassert chip select

//     gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,0);  // Assert chip select again
//     spi_hal_updateDataAndTransfer(&AS5048A_spiInst,zposl_command,1,WRITE); 
    
//     spi_hal_updateDataAndTransfer(&AS5048A_spiInst,current_angle,1,WRITE);     // Write LSB
//     gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,1);  // Deassert chip select

//     // Step 5: Burn the zero position data to OTP
//     gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,0);  // Assert chip select (active low)
//     uint16_t otp_control_command = AS5048A_PROGCTL_REG | AS5048A_PROGRAM_ENABLE;  // Enable OTP programming
//     otp_control_command |= calculate_even_parity(otp_control_command);  // Add parity bit
//     spi_hal_updateDataAndTransfer(&AS5048A_spiInst,otp_control_command,1,WRITE);   // Enable programming
//     otp_control_command |= AS5048A_BURN;  // Set the Burn bit
//     spi_hal_updateDataAndTransfer(&AS5048A_spiInst,otp_control_command,1,WRITE);  // Start burning OTP
    
//     gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,1);  // Deassert chip select

//     // Step 6: Read angle after programming to check if zero position is set
//     uint16_t angle_after_burn = AS5048AReadAngle();
//     if (angle_after_burn != 0) {
//         // printf("Error: Angle after burn is not 0. Zero position setting failed.\n");
//         return;
//     }
//     // Step 7: Set the Verify bit to load OTP data into the internal registers
//     gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,0);  // Assert chip select
//     otp_control_command |= AS5048A_VERIFY;  // Set the Verify bit

//     spi_hal_updateDataAndTransfer(&AS5048A_spiInst,otp_control_command,1,WRITE);  // Enable OTP verification
//     gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,1);  // Deassert chip select

//     // Step 8: Verify by reading the angle again to ensure it's set to 0
//     uint16_t angle_after_verify = AS5048AReadAngle();
//     if (angle_after_verify == 0) {
//         // printf("Zero position programmed successfully.\n");
//     } else {
//         // printf("Error: Verification failed. Zero position not properly set.\n");
//     }
// }
