#include "ds18b20.h"

#include <util/delay.h>

#define DS_SET_INPUT(conn) (*(conn->ddr) &= ~(1 << conn->pinNum))
#define DS_SET_OUTPUT(conn) (*(conn->ddr) |= (1 << conn->pinNum))

#define DS_LOW(conn) (*(conn->port) &= ~(1 << conn->pinNum))
#define DS_HIGH(conn) (*(conn->port) |= (1 << conn->pinNum))

#define DS_READ(conn) (*(conn->pin) & (1 << conn->pinNum))

typedef enum {
	DS_CONVERTTEMP = 0x44,
	DS_RSCRATCHPAD = 0xBE,
	DS_WSCRATCHPAD = 0x4E,
	DS_CPYSCRATCHPAD = 0x48,
	DS_RECEEPROM = 0xB8,
	DS_RPWRSUPPLY = 0xB4,
	DS_SEARCHROM = 0xF0,
	DS_READROM = 0x33,
	DS_MATCHROM = 0x55,
	DS_SKIPROM = 0xCC,
	DS_ALARMSEARCH = 0xEC
} ds_op;

uint8_t ds18b20_internal_readBit(ds18b20_conn *conn);
void ds18b20_internal_writeBit(ds18b20_conn *conn, uint8_t bit);

uint8_t ds18b20_internal_readByte(ds18b20_conn *conn);
void ds18b20_internal_writeByte(ds18b20_conn *conn, uint8_t data);

uint8_t ds18b20_reset(ds18b20_conn *conn) {
	uint8_t val = 0xFF;

	// The master pulls the line low for a minimum of 480uS and then releases the line
	DS_LOW(conn);
	DS_SET_OUTPUT(conn);
	_delay_us(480);

	// Wait for 60uS for the sensor to receive the rising edge (given by pull-up resistor)
	DS_SET_INPUT(conn);
	_delay_us(60);

	// Get line value and end the waiting
	val = DS_READ(conn);
	_delay_us(420);

	return val;
}

void ds18b20_readROM(ds18b20_conn *conn, uint8_t buf[8]) {
	ds18b20_reset(conn);
	ds18b20_internal_writeByte(conn, DS_READROM);

	for (uint8_t counter = 0; counter < 8; counter++)
		buf[counter] = ds18b20_internal_readByte(conn);

	ds18b20_reset(conn);
}

// *************

uint8_t ds18b20_internal_readBit(ds18b20_conn *conn) {
	uint8_t bit = 0;

	// Pull line low for 1uS
	DS_LOW(conn);
	DS_SET_OUTPUT(conn);
	_delay_us(1);

	// Release and wait for 14uS
	DS_SET_INPUT(conn);
	_delay_us(14);

	bit = DS_READ(conn) ? 1 : 0;

	// Wait 45uS to end the procedure and return
	_delay_us(45);

	return bit;
}

void ds18b20_internal_writeBit(ds18b20_conn *conn, uint8_t bit) {
	// Pull line to low for 1uS
	DS_LOW(conn);
	DS_SET_OUTPUT(conn);
	_delay_us(1);

	// When we want to write 1, we release the line
	if (bit) DS_SET_INPUT(conn);

	// Wait 60uS and release
	_delay_us(45);
	DS_SET_INPUT(conn);
}

uint8_t ds18b20_internal_readByte(ds18b20_conn *conn) {
	uint8_t byte = 0;

	for (uint8_t counter = 0; counter < 8; counter++) {
		byte >>= 1;
		byte |= ds18b20_internal_readBit(conn) << 7;
	}

	return byte;
}

void ds18b20_internal_writeByte(ds18b20_conn *conn, uint8_t data) {
	for (uint8_t counter = 0; counter < 8; counter++) {
		ds18b20_internal_writeBit(conn, data & 1);
		data >>= 1;
	}

}

