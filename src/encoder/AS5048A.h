#ifndef AS5048A_H
#define AS5048A_H


// #define 
#define AS5048A_ANGLE_REG 0x3FFF
#define AS5048A_MAGNUTUDE_REG 0xFFE
#define AS5048A_RW 0x4000
#define AS5048A_PARITY 0x8000
#define AS5048A_DATA_BITS 16
#define AS5048A_BITORDER 1//MSB FIRST
#define AS5048A_SPI_INSTANCE spi0
#define AS5048A_SCK 18 //GP18 blue
#define AS5048A_TX 19 //GP19 yellow
#define AS5048A_RX 16 //GP16 green
#define AS5048A_CS 17 //GP17 white


#define AS5048A_CPR 16384
#define AS5048A_ERROR_REG 0x0001
#define AS5048A_PROGCTL_REG 0x0003
#define AS5048A_OTPHIGH_REG 0x0016
#define AS5048A_OTPLOW_REG 0x0017
#define AS5048A_DIAGNOSTICS_REG 0x3FFD
#define AS5048A_MAGNITUDE_REG 0x3FFE
#define AS5048A_PARITY 0x8000
#define AS5048A_RW 0x4000
#define AS5048A_ERRFLG 0x4000
#define AS5048A_RESULT_MASK 0x3FFF

#define AS5048A_PROGRAM_ENABLE 0x01
#define AS5048A_BURN 0x08
#define AS5048A_VERIFY 0x0040
#define AS5048A_ANGLE_CMD 0xFFFF

#define WRITE 1
#define READ 0

void AS5048AInit(void);
void AS5048ASetConfigReg(void);
void AS5048AReadAngleMeasurement(void);
float AS5048AProcessAngleMeasurement(void);



#endif
