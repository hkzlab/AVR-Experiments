#include "twilib.h"

#include <util/twi.h>
#include <stdlib.h>

#define MAX_RETRIES 20

typedef enum {
	I2C_StartCommand = 0,
	I2C_DataCommand = 1,
	I2C_StopCommand = 2
} I2C_CommandType;

uint8_t I2C_internal_sendCommand(I2C_CommandType command);

I2C_Config *I2C_buildDefaultConfig(void) {
	I2C_Config *cfg = (I2C_Config*)malloc(sizeof(I2C_Config));

	cfg->psc = I2C_Psc_1;
	cfg->twbr_val = 0x30;

	return cfg;
}

void I2C_masterBegin(I2C_Config* config) {
	TWSR = (TWSR & 0xFC) | config->psc;
	TWBR = config->twbr_val;
}

// TWINT is the interrupt flag bit
// TWSTA is the start condition bit
// TWEN is the enable-TWI bit
// TWSTO is the stop-TWI bit

// TWCR is the TWI control register

uint8_t I2C_internal_sendCommand(I2C_CommandType command) {
	switch (command) {
		case I2C_StartCommand:
			TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
			break;
		case I2C_DataCommand:
			TWCR = (1 << TWINT) | (1 << TWEN);
			break;
		case I2C_StopCommand:
		default:
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
			return 0;		
	}

	while (!(TWCR & (1 << TWINT))); // Wait until TWINT flag gets set in TWCR, meaning that the command was received.

	// Return the value in TWSR, masking the last bits (the prescaling)
	return (TWSR & 0xF8);
}

int I2C_masterReadRegisterByte(I2C_Device *dev, uint8_t reg, uint8_t *data) {
	uint8_t twiStatus;
	uint8_t retryCount = 0;

	do {
		twiStatus = I2C_internal_sendCommand(I2C_StartCommand);
		retryCount++;
	} while(twiStatus == TW_MT_ARB_LOST && retryCount <= MAX_RETRIES);
	retryCount--; // If we got out, we have one retryCount too many

	if ((twiStatus != TW_START) && (twiStatus != TW_REP_START)) {
		twiStatus = I2C_internal_sendCommand(I2C_StopCommand);
		return -1;
	}

	// Now send slave address...
}
