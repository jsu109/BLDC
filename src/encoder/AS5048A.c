#include "stdint.h"
#include "AS5048A.h"
#include "spi_hal.h"
#include "gpio_hal.h"

typedef struct {
    
} AS5048A_t;

static AS5048A_t AS5048A;

//define settings for SPI 
SPI_settings_t AS5048A_spiSettings = {
.sysType = RP2040,
.baudrate = 80000,
.dataBits = AS5048A_DATA_BITS,
.order = AS5048A_BITORDER,
.cpol = 0,
.cpha = 1,
.cpha = 1,
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
    
 void AS5048AInit(void)
{
    // Initialization settings
    AS5048A_spiInst.settings = AS5048A_spiSettings;
    AS5048A_SCK_gpioInst.settings = AS5048A_SCK_settings;
    AS5048A_TX_gpioInst.settings = AS5048A_TX_settings;
    AS5048A_RX_gpioInst.settings = AS5048A_RX_settings;
    AS5048A_CS_gpioInst.settings = AS5048A_CS_settings;

    //initalise GPIO being used for AS5048A
    gpio_hal_init(&AS5048A_SCK_gpioInst,&AS5048A_SCK_settings);
    gpio_hal_init(&AS5048A_TX_gpioInst,&AS5048A_TX_settings);
    gpio_hal_init(&AS5048A_RX_gpioInst,&AS5048A_RX_settings);
    gpio_hal_init(&AS5048A_CS_gpioInst,&AS5048A_CS_settings);
    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_settings,1);

    //initalise SPI for AS5048A
    spi_hal_init(&AS5048A_spiInst, &AS5048A_spiSettings);
    spi_hal_config(&AS5048A_spiInst,&AS5048A_spiSettings);
    
}

 void AS5048ASetConfigReg(void) 
{
    // configure settings in AS5048A
 
    
}

uint16_t AS5048AReadAngle() {
    // Construct the command with required flags
    uint16_t command = AS5048A_ANGLE_REG | AS5048A_PARITY | AS5048A_RW;
    uint16_t result = 0;
    AS5048A_spiInst.data.cmd = command;
    AS5048A_spiInst.data.res = result;
    AS5048A_spiInst.data.len = 1;
    AS5048A_spiInst.data.RW = 0;  //read

    // Assert chip select (active low)
    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,0);

    // Send the command and read the response 
    spi_hal_updateDataAndTransfer(&AS5048A_spiInst,command,result,1,READ); 
    // Deassert chip select
    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,1);
    // Mask the result to retrieve only the relevant bits (14-bit angle)
    result = AS5048A_spiInst.data.res;

    return result & AS5048A_RESULT_MASK;
}
// Function to calculate parity (odd parity)
uint16_t calculate_parity(uint16_t data) {
    uint16_t parity = 0;
    for (int i = 0; i < 15; i++) { // Ignore MSB
        if (data & (1 << i)) {
            parity ^= 1;
        }
    }
    return (parity << 15); // Parity bit at MSB
}

void setZero()
{
    // Step 1: Construct commands for ZPOSM and ZPOSL
    uint16_t zposm_command = AS5048A_OTPHIGH_REG | !AS5048A_RW;  // Write command for MSB
    zposm_command |= calculate_parity(zposm_command);           // Add parity bit

    uint16_t zposl_command = AS5048A_OTPLOW_REG | !AS5048A_RW;  // Write command for LSB
    zposl_command |= calculate_parity(zposl_command);           // Add parity bit

    // Step 2: Write 0x0000 to OTP registers to clear previous zero position
    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,0);  // Assert chip select (active low)
    
    spi_hal_updateDataAndTransfer(&AS5048A_spiInst,zposm_command,NULL,1,WRITE); 
    
    spi_hal_updateDataAndTransfer(&AS5048A_spiInst,0x0000,NULL,1,WRITE); //clear MSB 

    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,1);  // Deassert chip select

    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,0);  // Assert chip select again
    spi_hal_updateDataAndTransfer(&AS5048A_spiInst,zposl_command,NULL,1,WRITE); //clear MSB
    spi_hal_updateDataAndTransfer(&AS5048A_spiInst,0x0000,NULL,1,WRITE);  // Clear LSB
    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,1);  // Deassert chip select

    // Step 3: Read the current angle from the sensor
    uint16_t current_angle = as5048aReadAngle();

    // Step 4: Write the current angle to OTP registers to set new zero position
    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,0);  // Assert chip select (active low)
    spi_hal_updateDataAndTransfer(&AS5048A_spiInst,zposm_command,NULL,1,WRITE); 
    
    spi_hal_updateDataAndTransfer(&AS5048A_spiInst,current_angle,NULL,1,WRITE);  

    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,1);  // Deassert chip select

    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,0);  // Assert chip select again
    spi_hal_updateDataAndTransfer(&AS5048A_spiInst,zposl_command,NULL,1,WRITE); 
    
    spi_hal_updateDataAndTransfer(&AS5048A_spiInst,current_angle,NULL,1,WRITE);     // Write LSB
    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,1);  // Deassert chip select

    // Step 5: Burn the zero position data to OTP
    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,0);  // Assert chip select (active low)
    uint16_t otp_control_command = AS5048A_PROGCTL_REG | AS5048A_PROGRAM_ENABLE;  // Enable OTP programming
    otp_control_command |= calculate_parity(otp_control_command);  // Add parity bit
    spi_hal_updateDataAndTransfer(&AS5048A_spiInst,otp_control_command,NULL,1,WRITE);   // Enable programming
    otp_control_command |= AS5048A_BURN;  // Set the Burn bit
    spi_hal_updateDataAndTransfer(&AS5048A_spiInst,otp_control_command,NULL,1,WRITE);  // Start burning OTP
    
    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,1);  // Deassert chip select

    // Step 6: Read angle after programming to check if zero position is set
    uint16_t angle_after_burn = as5048aReadAngle();
    if (angle_after_burn != 0) {
        printf("Error: Angle after burn is not 0. Zero position setting failed.\n");
        return;
    }
    // Step 7: Set the Verify bit to load OTP data into the internal registers
    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,0);  // Assert chip select
    otp_control_command |= AS5048A_VERIFY;  // Set the Verify bit

    spi_hal_updateDataAndTransfer(&AS5048A_spiInst,otp_control_command,NULL,1,WRITE);  // Enable OTP verification
    gpio_hal_put(&AS5048A_CS_gpioInst,&AS5048A_CS_gpioInst.settings,1);  // Deassert chip select

    // Step 8: Verify by reading the angle again to ensure it's set to 0
    uint16_t angle_after_verify = as5048aReadAngle();
    if (angle_after_verify == 0) {
        printf("Zero position programmed successfully.\n");
    } else {
        printf("Error: Verification failed. Zero position not properly set.\n");
    }
}
