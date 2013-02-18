#include "hd44780-highlevel.h"
#include "hd44780-commands.h"

#include <util/delay.h>

void hd44780_hl_init(hd44780_driver *driver, uint8_t show_cursor, uint8_t blink_cursor) {
	if (!driver) return;
	
	uint8_t interfaceType = driver->initialize(driver->conn_struct);

	uint8_t linesType;
	uint8_t fontsType;

	switch (driver->type) {
		case hd44780_16x1:
			linesType = 1;
			fontsType = 0;
			break;
		default:
			break;
	}

	// Setup display
	driver->sendCommand(driver->conn_struct, hd44780_FunctionSet((interfaceType == 4) ? 0 : 1, linesType, fontsType));

	// Setup the cursor
	driver->sendCommand(driver->conn_struct, hd44780_DisplayControl(0, 0, 0));

	// Clear the display
	driver->sendCommand(driver->conn_struct, hd44780_ClearDisplay());

	// Reset shift
	driver->sendCommand(driver->conn_struct, hd44780_EntryModeSet(1, 0));

	// Enable display and blinking
	driver->sendCommand(driver->conn_struct, hd44780_DisplayControl(1, show_cursor ? 1 : 0, blink_cursor ? 1 : 0));
} 

void hd44780_hl_printText(hd44780_driver *driver, uint8_t line, uint8_t position, char *text) {
	if (!driver) return;
	uint8_t char_address = 0;

	switch (driver->type) {
		case hd44780_16x1:
			position = (0x40 * line) + position;
			char_address = (position % 8) + (0x40 * (position / 8));
			break;
		default:
			break;
	}

	// Move to position
	driver->sendCommand(driver->conn_struct, hd44780_SetDDRAMAddr(char_address));

	for (int idx = 0; text[idx] != '\0'; idx++) {
		driver->sendCommand(driver->conn_struct, hd44780_WriteData(text[idx]));
		_delay_ms(1);

		position++;
		
		switch (driver->type) {
			case hd44780_16x1:
				if (!(position % 8) && (position / 8)) {
					char_address = (position % 8) + (0x40 * (position / 8));
					driver->sendCommand(driver->conn_struct, hd44780_SetDDRAMAddr(char_address));
				}
				break;
			default:
				break;
		}
	}
}

void hd44780_hl_printChar(hd44780_driver *driver, uint8_t line, uint8_t position, char c) {
	if (!driver) return;
	uint8_t char_address = 0;

	switch (driver->type) {
		case hd44780_16x1:
			position = (0x40 * line) + position;
			char_address = (position % 8) + (0x40 * (position / 8));
			break;
		default:
			break;
	}

	// Move to position
	driver->sendCommand(driver->conn_struct, hd44780_SetDDRAMAddr(char_address));

	// Print char
	driver->sendCommand(driver->conn_struct, hd44780_WriteData(c));
}

void hd44780_hl_shiftDisplay(hd44780_driver *driver, uint8_t direction) {
	if (!driver) return;

	driver->sendCommand(driver->conn_struct, hd44780_SetShift(1, direction ? 1 : 0));
}

void hd44780_hl_shiftCursor(hd44780_driver *driver, uint8_t direction) {
	if (!driver) return;

	driver->sendCommand(driver->conn_struct, hd44780_SetShift(0, direction ? 1 : 0));
}

void hd44780_hl_setCustomFont(hd44780_driver *driver, uint8_t slot, uint8_t *data) {
	if (!driver) return;
	
	if (slot > 7) return; // hd44780 specifies 8 slots only for custom characters

	driver->sendCommand(driver->conn_struct, hd44780_SetCGRAMAddr(slot * 8));

	for (uint8_t idx = 0; idx < 8; idx++) {
		driver->sendCommand(driver->conn_struct, hd44780_WriteData(data[idx]));
	}

	driver->sendCommand(driver->conn_struct, hd44780_SetDDRAMAddr(0));
}
