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

//

static void internal_spiChipSelect(uint8_t state);

/****/
void mcp2515_simpleStartup(mcp2515_canspeed speed, uint8_t loopback) {
	mcp2515_reset(); // Reset the MCP
	mcp2515_setMode(mcp_func_config); // Make sure it's in config mode

	mcp2515_setCanSpeed(speed); // Set the speed
	mcp2515_writeRegister(MCP2515_REG_CANINTE, 0xA3); // Enable RX0/1, ERRIE and MERRE interrupts
	mcp2515_writeRegister(MCP2515_REG_CANINTF, 0x00); // Clear interrupt flags
	mcp2515_writeRegister(MCP2515_REG_EFLG, 0x00); // Clear error flags
	mcp2515_writeRegister(MCP2515_REG_TXRTSCTRL, 0x00); // Clear TXRTSCLR

	mcp2515_setMode(loopback ? mcp_func_loopback : mcp_func_normal); // Put it back in normal/loopback mode
}

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

void mcp2515_setupTX(mcp2515_txb txb, const uint8_t *addr, uint8_t dLen, uint8_t rtr, uint8_t priority) {
	// Enable the chip
	internal_spiChipSelect(1);
	
	// Send WRITE instruction, register address and values
	send_spi(MCP2515_INSTR_WRITE);
	send_spi(MCP2515_REG_TXB0SIDH + txb);

	send_spi(addr[0]); // SIDH
	send_spi(addr[1]); // SIDL
	send_spi(addr[2]); // EID8
	send_spi(addr[3]); // EID0
	send_spi(((rtr ? 1 : 0) << 6)|(dLen & 0x0F)); // dLen + RTR

	// Disable the chip
	internal_spiChipSelect(0);

	mcp2515_bitModify(MCP2515_REG_TXB0CTRL + txb, priority, 0x03);
}

void mcp2515_setupRX(mcp2515_rxb rxb, const uint8_t *filter, const uint8_t *mask) {
	// Enable the chip
	internal_spiChipSelect(1);

	// WRITE the filter
	send_spi(MCP2515_INSTR_WRITE);
	send_spi(MCP2515_REG_RXF0SIDH + rxb);
	send_spi(filter[0]); // SIDH
	send_spi(filter[1]); // SIDL
	send_spi(filter[2]); // EID8
	send_spi(filter[3]); // EID0

	// Disable & Enable the chip
	internal_spiChipSelect(0);
	internal_spiChipSelect(1);

	// WRITE the mask
	send_spi(MCP2515_INSTR_WRITE);
	send_spi(MCP2515_REG_RXM0SIDH + rxb);
	send_spi(mask[0]); // SIDH
	send_spi(mask[1]); // SIDL
	send_spi(mask[2]); // EID8
	send_spi(mask[3]); // EID0

	// Disable the chip
	internal_spiChipSelect(0);
}

uint8_t mcp2515_readMSG(mcp2515_rxb rxb, uint8_t *address, uint8_t *data) {
	uint8_t size = 0, idx;

	// Enable the chip
	internal_spiChipSelect(1);

	// Read the address...
	send_spi(MCP2515_INSTR_READ);
	send_spi(MCP2515_REG_RXB0SIDH + rxb);
	address[0] = send_spi(0x00); // SIDH
	address[1] = send_spi(0x00); // SIDL
	address[2] = send_spi(0x00); // EID8
	address[3] = send_spi(0x00); // EID0
	address[4] = send_spi(0x00); // DLC

	// Get the packet size
	size = address[4] & 0x0F;

	// And read the packet
	for (idx = 0; idx < size; idx++)
		data[idx] = send_spi(0x00);

	// Disable the chip
	internal_spiChipSelect(0);

	return size;
}

void mcp2515_loadMSG(mcp2515_txb txb, uint8_t *data, uint8_t len) {
	uint8_t idx;

	// Enable the chip
	internal_spiChipSelect(1);

	// Send WRITE instruction, register address and values
	send_spi(MCP2515_INSTR_WRITE);
	send_spi(MCP2515_REG_TXB0D0 + txb);

	for(idx = 0; idx < len; idx++)
		send_spi(data[idx]);

	// Disable the chip
	internal_spiChipSelect(0);
}

void mcp2515_sendMSG(uint8_t buffers) {
	// Enable the chip
	internal_spiChipSelect(1);

	send_spi(MCP2515_INSTR_RTS | buffers);

	// Disable the chip
	internal_spiChipSelect(0);
}

void mcp2515_setMode(mcp2515_func_mode mode) {
	mcp2515_bitModify(MCP2515_REG_CANCTRL, mode, 0xE0);
	while((mcp2515_readRegister(MCP2515_REG_CANSTAT) & 0xE0) != mode);
}

uint8_t mcp2515_setBitTiming(uint8_t rCNF1, uint8_t rCNF2, uint8_t rCNF3) {
	if ((mcp2515_readRegister(MCP2515_REG_CANSTAT) & 0xE0) == mcp_func_config) {
		mcp2515_writeRegister(MCP2515_REG_CNF1, rCNF1);
		mcp2515_writeRegister(MCP2515_REG_CNF2, rCNF2);
		mcp2515_bitModify(MCP2515_REG_CNF3, rCNF3, /*0xC7*/0x07);

		return 1;
	} else {
		return 0;
	}
}

uint8_t mcp2515_setCanSpeed(mcp2515_canspeed speed) {
	uint8_t rCNF1, rCNF2, rCNF3;

	// Value calculated with microchip bit-timing tool
	switch (speed) {
		case mcp_can_speed_500:
			rCNF1 = 0x00;
			rCNF2 = 0xBA;
			rCNF3 = 0x07;
			break;
		case mcp_can_speed_250:
			rCNF1 = 0x01;
			rCNF2 = 0xBA;
			rCNF3 = 0x07;
			break;
		case mcp_can_speed_125:
			rCNF1 = 0x03;
			rCNF2 = 0xBA;
			rCNF3 = 0x07;
			break;
		case mcp_can_speed_50:
		default:
			rCNF1 = 0x07;
			rCNF2 = 0xBF;
			rCNF3 = 0x07;
			break;

	}

	return mcp2515_setBitTiming(rCNF1, rCNF2, rCNF3);
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

