#ifndef AS5048A_H
#define AS5048A_H


// #define 
#define AS5048A_ANGLE_REG 0xFFF
#define AS5048A_MAGNUTUDE_REG 0xFFE
#define AS5048A_RW 0x4000
#define AS5048A_PARITY 0x8000
#define AS5048A_DATA_BITS 16
#define AS5048A_BITORDER 1//MSB FIRST
#define SPI0_CSn 6
#define AS5048A_SPI_INSTANCE spi0
#define AS5048A_CS SPI0_CSn
void AS5048AInit(void);
void AS5048ASetConfigReg(void);
void AS5048AReadAngleMeasurement(void);
float AS5048AProcessAngleMeasurement(void);



#endif
