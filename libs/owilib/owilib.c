#include "owilib.h"

#include <util/delay.h>

#define OWI_SET_INPUT(conn) (*(conn->ddr) &= ~(1 << conn->pinNum))
#define OWI_SET_OUTPUT(conn) (*(conn->ddr) |= (1 << conn->pinNum))

#define OWI_LOW(conn) (*(conn->port) &= ~(1 << conn->pinNum))
#define OWI_HIGH(conn) (*(conn->port) |= (1 << conn->pinNum))

#define OWI_READ(conn) (*(conn->pin) & (1 << conn->pinNum))

typedef enum {
	OWI_CONVERTTEMP = 0x44,
	OWI_RSCRATCHPAD = 0xBE,
	OWI_WSCRATCHPAD = 0x4E,
	OWI_CPYSCRATCHPAD = 0x48,
	OWI_RECEEPROM = 0xB8,
	OWI_RPWRSUPPLY = 0xB4,
	OWI_SEARCHROM = 0xF0,
	OWI_READROM = 0x33,
	OWI_MATCHROM = 0x55,
	OWI_SKIPROM = 0xCC,
	OWI_ALARMSEARCH = 0xEC
} ds_op;

uint8_t owi_internal_readBit(owi_conn *conn);
void owi_internal_writeBit(owi_conn *conn, uint8_t bit);

uint8_t owi_internal_readByte(owi_conn *conn);
void owi_internal_writeByte(owi_conn *conn, uint8_t data);

uint8_t owi_reset(owi_conn *conn) {
	uint8_t val = 0xFF;

	// The master pulls the line low for a minimum of 480uS and then releases the line
	OWI_LOW(conn);
	OWI_SET_OUTPUT(conn);
	_delay_us(480);

	// Wait for 60uS for the sensor to receive the rising edge (given by pull-up resistor)
	OWI_SET_INPUT(conn);
	_delay_us(60);

	// Get line value and end the waiting
	val = OWI_READ(conn);
	_delay_us(420);

	return val;
}

void owi_readROM(owi_conn *conn, uint8_t buf[8]) {
	owi_reset(conn);
	owi_internal_writeByte(conn, OWI_READROM);

	for (uint8_t counter = 0; counter < 8; counter++)
		buf[counter] = owi_internal_readByte(conn);

	owi_reset(conn);
}

// *************

uint8_t owi_internal_readBit(owi_conn *conn) {
	uint8_t bit = 0;

	// Pull line low for 1uS
	OWI_LOW(conn);
	OWI_SET_OUTPUT(conn);
	_delay_us(1);

	// Release and wait for 14uS
	OWI_SET_INPUT(conn);
	_delay_us(14);

	bit = OWI_READ(conn) ? 1 : 0;

	// Wait 45uS to end the procedure and return
	_delay_us(45);

	return bit;
}

void owi_internal_writeBit(owi_conn *conn, uint8_t bit) {
	// Pull line to low for 1uS
	OWI_LOW(conn);
	OWI_SET_OUTPUT(conn);
	_delay_us(1);

	// When we want to write 1, we release the line
	if (bit) OWI_SET_INPUT(conn);

	// Wait 60uS and release
	_delay_us(45);
	OWI_SET_INPUT(conn);
}

uint8_t owi_internal_readByte(owi_conn *conn) {
	uint8_t byte = 0;

	for (uint8_t counter = 0; counter < 8; counter++) {
		byte >>= 1;
		byte |= owi_internal_readBit(conn) << 7;
	}

	return byte;
}

void owi_internal_writeByte(owi_conn *conn, uint8_t data) {
	for (uint8_t counter = 0; counter < 8; counter++) {
		owi_internal_writeBit(conn, data & 1);
		data >>= 1;
	}

}

