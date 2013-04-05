#include "owilib.h"
#include "stdio.h"

#include <stdio.h>

#include <util/delay.h>

#define OWI_SET_INPUT(conn) (*(conn->ddr) &= ~(1 << conn->pinNum))
#define OWI_SET_OUTPUT(conn) (*(conn->ddr) |= (1 << conn->pinNum))

#define OWI_LOW(conn) (*(conn->port) &= ~(1 << conn->pinNum))
#define OWI_HIGH(conn) (*(conn->port) |= (1 << conn->pinNum))

#define OWI_READ(conn) (*(conn->pin) & (1 << conn->pinNum))

typedef enum {
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
	owi_writeByte(conn, OWI_READROM);

	for (uint8_t counter = 0; counter < 8; counter++)
		buf[counter] = owi_readByte(conn);

	owi_reset(conn);
}

void owi_searchROM(owi_conn *conn, uint8_t *buf, uint8_t *count) {
	uint8_t temp_address[8];

	uint8_t discrepancy = 0, lastDiscrepancy = 0;
	uint8_t romBitIdx;
	uint8_t done = 0;

	uint8_t bitA, bitB;

	uint8_t presence = !owi_reset(conn);
	if (!presence) {
		*count = 0;

		return;
	}

	*count = 0;


	uint8_t anotherROMAvailable;
	do {
		for (uint8_t idx = 0; idx < 8; idx++) {
			temp_address[idx] = 0;
		}

		anotherROMAvailable = 0;

		if (done) {
			done = 0;
			continue;
		}

		presence = !owi_reset(conn);
		if (!presence) {
			lastDiscrepancy = 0;
			continue;
		}

		romBitIdx = 1;
		discrepancy = 0;

		owi_writeByte(conn, OWI_SEARCHROM);

		while (1) {
			bitA = owi_internal_readBit(conn);
			bitB = owi_internal_readBit(conn);

			if (bitB && bitA) {
				lastDiscrepancy = 0;
				break;
			}

			if (!bitB && !bitA) {
				if (romBitIdx == lastDiscrepancy) {
					temp_address[((romBitIdx - 1) / 8)] |= (1 << ((romBitIdx - 1) % 8));

					owi_internal_writeBit(conn, 1);
					romBitIdx++;

					if (romBitIdx > 64) {
						lastDiscrepancy = discrepancy;
						if (!lastDiscrepancy) done = 1;
						anotherROMAvailable = 1;
						break;
					} else {
						continue;
					}
				} else if (romBitIdx > lastDiscrepancy) {
					temp_address[((romBitIdx - 1) / 8)] &= ~(1 << ((romBitIdx - 1) % 8));

					discrepancy = romBitIdx;
					owi_internal_writeBit(conn, 0);
					romBitIdx++;

					if (romBitIdx > 64) {
						lastDiscrepancy = discrepancy;
						if (!lastDiscrepancy) done = 1;
						anotherROMAvailable = 1;
						break;
					} else {
						continue;
					}
				} else {
					uint8_t curRomBit = (temp_address[((romBitIdx - 1) / 8)] >> ((romBitIdx - 1) % 8)) & 1;

					if (!curRomBit)
						discrepancy = romBitIdx;

					owi_internal_writeBit(conn, curRomBit);
					romBitIdx++;

					if (romBitIdx > 64) {
						lastDiscrepancy = discrepancy;
						if (!lastDiscrepancy) done = 1;
						anotherROMAvailable = 1;
						break;
					} else {
						continue;
					}
				}
			} else {
				if (bitA)
					temp_address[((romBitIdx - 1) / 8)] |= (1 << ((romBitIdx - 1) % 8));
				else
					temp_address[((romBitIdx - 1) / 8)] &= ~(1 << ((romBitIdx - 1) % 8));

				owi_internal_writeBit(conn, bitA);
				romBitIdx++;

				if (romBitIdx > 64) {
					lastDiscrepancy = discrepancy;
					if (!lastDiscrepancy) done = 1;
					anotherROMAvailable = 1;
					break;
				} else {
					continue;
				}
			}

		}

		if (bitA && bitB) continue;

		if (buf) {
			for (uint8_t idx = 0; idx < 8; idx++) {
				buf[idx + (*count) * 8] = temp_address[idx];
			}
		}
		(*count)++;

	} while (anotherROMAvailable);
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

uint8_t owi_readByte(owi_conn *conn) {
	uint8_t byte = 0;

	for (uint8_t counter = 0; counter < 8; counter++) {
		byte >>= 1;
		byte |= owi_internal_readBit(conn) << 7;
	}

	return byte;
}

void owi_writeByte(owi_conn *conn, uint8_t data) {
	for (uint8_t counter = 0; counter < 8; counter++) {
		owi_internal_writeBit(conn, data & 1);
		data >>= 1;
	}

}

