#ifndef _TWILIB_HEADER_
#define _TWILIB_HEADER_

#include <stdint.h>

// SCL freq = (CPU_FREQ) / (16 + 2 * TWBR * PRESCALER)

typedef enum {
	I2C_Psc_1 = 0,
	I2C_Psc_4 = 1,
	I2C_Psc_16 = 2,
	I2C_Psc_64 = 3
} I2C_Prescaler;

typedef struct {
	I2C_Prescaler psc;
	uint8_t twbr_val;
} I2C_Config;

typedef struct {
	uint8_t id; // Device id
	uint8_t address; // Device address
} I2C_Device;

// For reference see:
// http://www.ermicro.com/blog/?p=744
// http://arduino.cc/en/Reference/Wire
// http://www.embedds.com/programming-avr-i2c-interface/
// http://hackaday.com/2012/01/09/programming-avr-i2c-interface/
// http://tutorialelectronic.com/?p=202

I2C_Config *I2C_buildDefaultConfig(void);
void I2C_masterBegin(I2C_Config* config);

int I2C_masterReadRegisterByte(I2C_Device *dev, uint8_t reg, uint8_t *data);
int I2C_masterWriteRegisterByte(I2C_Device *dev, uint8_t reg, uint8_t data);

#endif /* _TWILIB_HEADER_ */
