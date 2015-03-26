#include "mcp2515.h"

#include "spi.h"

#ifdef __cplusplus
extern "C"{
#endif

/* Instruction primitives supported by MCP2515 via SPI:
 * - RESET: Reset the device
 * - READ: Read access to the register map
 * - READ RX BUF: Quick access to the register buffer
 * - WRITE: Write access to the register map
 * - LOAD TX BUF: Quick access to transmit buffer
 * - RTS: Requests to send CAN message in the selected buffer
 * - READ DEV STAT: Returns info about the device and buffer status
 * - READ RX STAT: Returns info about received messages
 * - BIT MODIFY: Change masked bits in selected register
 */

static void internal_spiChipSelect(uint8_t state);

/****/
uint8_t mcp2515_readRegister(uint8_t address) {
	uint8_t ret_buf;

	// Enable the chip
	internal_spiChipSelect(1);

	// Send READ instruction and register address
	send_spi(MCP2515_INSTR_READ);
	send_spi(address);

	// Read the result
	ret_buf = send_spi(0);

	// Disable the chip
	internal_spiChipSelect(0);

	return ret_buf;
}

void mcp2515_writeRegister(uint8_t address, uint8_t value) {
	// Enable the chip
	internal_spiChipSelect(1);

	// Send WRITE instruction, register address and value
	send_spi(MCP2515_INSTR_WRITE);
	send_spi(address);
	send_spi(value);

	// Disable the chip
	internal_spiChipSelect(0);
}

void mcp2515_bitModify(uint8_t address, uint8_t value, uint8_t mask) {
	// Enable the chip
	internal_spiChipSelect(1);

	// Send BITMOD instruction and register address
	send_spi(MCP2515_INSTR_BITMOD);
	send_spi(address); // This is where we want to change bits
	// Send mask
	send_spi(mask); // Which bits can be changed
	// Send data
	send_spi(value); // What value the bits will be changed to

	// Disable the chip
	internal_spiChipSelect(0);
}

uint8_t mcp2515_readStatus(void) {
	uint8_t ret_buf;

	// Enable the chip
	internal_spiChipSelect(1);

	// Send READSTAT instruction
	send_spi(MCP2515_INSTR_READSTAT);
	
	// Read the result
	ret_buf = send_spi(0);

	// Disable the chip
	internal_spiChipSelect(0);

	return ret_buf;
}

void mcp2515_reset(void) {
	// Enable the chip
	internal_spiChipSelect(1);
	
	send_spi(MCP2515_INSTR_RESET);

	// Disable the chip
	internal_spiChipSelect(0);
}

void mcp2515_rts(mcp2515_rts_buf buf) {
	// Enable the chip
	internal_spiChipSelect(1);
	
	send_spi(MCP2515_INSTR_RTS | buf);

	// Disable the chip
	internal_spiChipSelect(0);	
}

uint8_t mcp2515_readRX(mcp2515_read_buf buf) {
	uint8_t ret_buf;
	
	// Enable the chip
	internal_spiChipSelect(1);
	
	send_spi(MCP2515_INSTR_READRX | buf);
	
	// Read the result
	ret_buf = send_spi(0);

	// Disable the chip
	internal_spiChipSelect(0);	

	return ret_buf;
}

/****/

static void internal_spiChipSelect(uint8_t state) {
	if(!state) {
		/* Upper the CS pin */
		PORT_SPI |= (1<<SPI_SS_PIN);
		DDR_SPI |= (1<<SPI_SS_PIN);
	} else {
		/* Lower the CS pin */
		PORT_SPI &= ~(1<<SPI_SS_PIN);
		DDR_SPI |= (1<<SPI_SS_PIN);
	}
}

#ifdef __cplusplus
} // extern "C"
#endif

