#include "twilib.h"

#include <util/twi.h>
#include <stdlib.h>

#if defined(__AVR_AT90Tiny2313__) | defined(__AVR_ATtiny2313__) | defined(__AVR_ATtiny4313__)
#include "AVR310_USI_TWI_Master/USI_TWI_Master.h"
#define USI_TWI 1

#define TWI_GEN_CALL         0x00  // The General Call address is 0
#endif

#define MAX_RETRIES 20

typedef enum {
	I2C_StartCommand = 0,
	I2C_DataNACKCommand = 1,
	I2C_DataACKCommand = 2,
	I2C_StopCommand = 3
} I2C_CommandType;

uint8_t I2C_internal_sendCommand(I2C_CommandType command);

I2C_Config *I2C_buildDefaultConfig(void) {
#ifdef USI_TWI
	return NULL;
#else
	I2C_Config *cfg = (I2C_Config*)malloc(sizeof(I2C_Config));

	cfg->psc = I2C_Psc_1;
	cfg->twbr_val = 0x30;

	return cfg;
#endif
}

void I2C_masterBegin(I2C_Config* config) {
#ifdef USI_TWI
	USI_TWI_Master_Initialise();
#else
	TWSR = (TWSR & 0xFC) | config->psc;
	TWBR = config->twbr_val;
#endif
}

// TWINT is the interrupt flag bit
// TWSTA is the start condition bit
// TWEN is the enable-TWI bit
// TWSTO is the stop-TWI bit

// TWCR is the TWI control register
#ifndef USI_TWI
uint8_t I2C_internal_sendCommand(I2C_CommandType command) {
	switch (command) {
		case I2C_StartCommand:
			TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
			break;
		case I2C_DataNACKCommand:
			TWCR = (1 << TWINT) | (1 << TWEN);
			break;
		case I2C_DataACKCommand:
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
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
#endif

int I2C_masterWriteRegisterByte(I2C_Device *dev, uint8_t *reg, uint8_t rLen, uint8_t *data, uint8_t dLen) {
#ifdef USI_TWI
	uint8_t cmd_buf[16]; // For speed reasons, use a static buffer...
	cmd_buf[0] = (((dev->id << 4) & 0xF0) | ((dev->address << 1) & 0x0E)) | (FALSE << TWI_READ_BIT);
	
	for (uint8_t curReg = 0; curReg < rLen; curReg++) {
		cmd_buf[1 + curReg] = reg[curReg];
	}

	for (uint8_t bufIdx = 0; bufIdx < dLen; bufIdx++)
		cmd_buf[1 + rLen + bufIdx] = data[bufIdx];

	int retval = USI_TWI_Start_Transceiver_With_Data(cmd_buf, dLen + 1 + rLen);

	return (retval == TRUE) ? 1 : -1;
#else
	uint8_t twiStatus;
	uint8_t retryCount = 0;
	int retval = 1;

	while (retryCount < MAX_RETRIES) {
		// Start command
		twiStatus = I2C_internal_sendCommand(I2C_StartCommand);
		if (twiStatus == TW_MT_ARB_LOST) { // We need to retry...
			retryCount++;
			continue; // Begin to loop again
		} else if ((twiStatus != TW_START) && (twiStatus != TW_REP_START)) {
			retval = -1;
			break;
		}

		// Now send slave address... asking for a write
		TWDR = ((dev->id << 4) & 0xF0) | ((dev->address << 1) & 0x0E) | TW_WRITE;
		twiStatus = I2C_internal_sendCommand(I2C_DataNACKCommand);
		if ((twiStatus == TW_MT_SLA_NACK) || (twiStatus == TW_MT_ARB_LOST)) {
			retryCount++;
			continue;
		} else if (twiStatus != TW_MT_SLA_ACK) {
			retval = -1;
			break;
		}

		// Send register address
		for (int curReg = 0; curReg < rLen; curReg++) {
			TWDR = reg[curReg];
			twiStatus = I2C_internal_sendCommand(I2C_DataNACKCommand);
			if (twiStatus != TW_MT_DATA_ACK) {
				retval = -1;
				break;
			}		
		}
		if (retval == -1) break; // Something bad happened sending the register address
	
		// Data transmission
		for (int curData = 0; curData < dLen; curData++) {
			// Put data into register and transmit
			TWDR = data[curData];

			if (curData == (dLen - 1)) { // Last write... we need to send a NACK
				twiStatus = I2C_internal_sendCommand(I2C_DataNACKCommand);
				if (twiStatus != TW_MT_DATA_NACK) {
					retval = -1;
					break;
				}
			} else { // We have to perform other reads... send an ACK
				twiStatus = I2C_internal_sendCommand(I2C_DataACKCommand);
				if (twiStatus != TW_MT_DATA_ACK) {
					retval = -1;
					break;
				}
			}
		}

		// Transmission complete!
		break;
	}

	twiStatus = I2C_internal_sendCommand(I2C_StopCommand);	
	
	return 1; 
#endif
}

int I2C_masterReadRegisterByte(I2C_Device *dev, uint8_t *reg, uint8_t rLen, uint8_t *data, uint8_t dLen) {
#ifdef USI_TWI
	uint8_t cmd_buf[16];
	cmd_buf[0] = (((dev->id << 4) & 0xF0) | ((dev->address << 1) & 0x0E)) | (FALSE << TWI_READ_BIT);
	
	for (int curReg = 0; curReg < rLen; curReg++) {
		cmd_buf[1 + curReg] = reg[curReg];
	}

	int retval = USI_TWI_Start_Transceiver_With_Data(cmd_buf, 1 + rLen);

	if (retval != TRUE) return -1;

	cmd_buf[0] = (((dev->id << 4) & 0xF0) | ((dev->address << 1) & 0x0E)) | (TRUE << TWI_READ_BIT);	
	do {
		retval = USI_TWI_Start_Transceiver_With_Data(cmd_buf, 1 + dLen);
	} while(USI_TWI_Get_State_Info() == USI_TWI_NO_ACK_ON_ADDRESS);

	for (uint8_t bufIdx = 0; bufIdx < dLen; bufIdx++)
		data[bufIdx] = cmd_buf[bufIdx + 1];

	return (retval == TRUE) ? 1 : -1;
#else
	uint8_t twiStatus;
	uint8_t retryCount = 0;
	int retval = 1;

	while (retryCount < MAX_RETRIES) {
		// Start command
		twiStatus = I2C_internal_sendCommand(I2C_StartCommand);
		if (twiStatus == TW_MT_ARB_LOST) { // We need to retry...
			retryCount++;
			continue; // Begin to loop again
		} else if ((twiStatus != TW_START) && (twiStatus != TW_REP_START)) {
			retval = -1;
			break;
		}

		// Now send slave address... asking for a write
		TWDR = ((dev->id << 4) & 0xF0) | ((dev->address << 1) & 0x0E) | TW_WRITE;
		twiStatus = I2C_internal_sendCommand(I2C_DataNACKCommand);
		if ((twiStatus == TW_MT_SLA_NACK) || (twiStatus == TW_MT_ARB_LOST)) {
			retryCount++;
			continue;
		} else if (twiStatus != TW_MT_SLA_ACK) {
			retval = -1;
			break;
		}

		// Send register address
		for (int curReg = 0; curReg < rLen; curReg++) {
			TWDR = reg[curReg];
			twiStatus = I2C_internal_sendCommand(I2C_DataNACKCommand);
			if (twiStatus != TW_MT_DATA_ACK) {
				retval = -1;
				break;
			}		
		}
		if (retval == -1) break; // Something bad happened sending the register address

		// Send start command again.
		twiStatus = I2C_internal_sendCommand(I2C_StartCommand);
		if (twiStatus == TW_MT_ARB_LOST) { // We need to retry...
			retryCount++;
			continue; // Begin to loop again
		} else if ((twiStatus != TW_START) && (twiStatus != TW_REP_START)) {
			retval = -1;
			break;
		}

		// Now send slave address... asking for a read
		TWDR = ((dev->id << 4) & 0xF0) | ((dev->address << 1) & 0x0E) | TW_READ;
		twiStatus = I2C_internal_sendCommand(I2C_DataNACKCommand);
		if ((twiStatus == TW_MR_SLA_NACK) || (twiStatus == TW_MR_ARB_LOST)) {
			retryCount++;
			continue;
		} else if (twiStatus != TW_MR_SLA_ACK) {
			retval = -1;
			break;
		}

		for (int curData = 0; curData < dLen; curData++) {
			// Read the data

			if (curData == (dLen - 1)) { // Last read... we need to send a NACK
				twiStatus = I2C_internal_sendCommand(I2C_DataNACKCommand);
				if (twiStatus != TW_MR_DATA_NACK) {
					retval = -1;
					break;
				}
			} else { // We have to perform other reads... send an ACK
				twiStatus = I2C_internal_sendCommand(I2C_DataACKCommand);
				if (twiStatus != TW_MR_DATA_ACK) {
					retval = -1;
					break;
				}
			}
		
			// Read the data!
			data[curData] = TWDR;
		}

		break;
	}

	twiStatus = I2C_internal_sendCommand(I2C_StopCommand);	
	return retval; 
#endif
}
