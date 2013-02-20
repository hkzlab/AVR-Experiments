#include "hd44780-highlevel.h"
#include "hd44780-commands.h"

#include <stdlib.h>

#include <util/delay.h>

#include <stdio.h>

hd44780_driver* hd44780_hl_createDriver(hd44780_type lcd_type, void *conn_struct, uint8_t (*initialize)(void*), void (*sendCommand)(void*, uint16_t)) {
	hd44780_driver *driver = (hd44780_driver*)malloc(sizeof(hd44780_driver));

	driver->type = lcd_type;
	driver->initialize = initialize;
	driver->sendCommand = sendCommand;
	driver->conn_struct = conn_struct;

	return driver;
}

void hd44780_hl_init(hd44780_driver *driver, uint8_t show_cursor, uint8_t blink_cursor) {
	if (!driver) return;
	
	uint8_t interfaceType = driver->initialize(driver->conn_struct);

	uint8_t linesType;
	uint8_t fontsType;

	switch (driver->type) {
		case PVC160101Q_16x1:
		case TMBC20464BSP_20x4:
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
		case PVC160101Q_16x1:
			position = position % 16;
			char_address = (position % 8) + (0x40 * (position / 8));
			break;
		case TMBC20464BSP_20x4:
			switch (line) {
				case 0:
					char_address = 0;
					break;
				case 1:
					char_address = 0x40;
					break;
				case 2:
					char_address = 0x14;
					break;
				case 3:
					char_address = 0x54;
					break;
				default:
					break;
			}

			position %= 20;			
			char_address += position;
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
		
		// Check for the beginning of a new LCD line
		switch (driver->type) {
			case PVC160101Q_16x1:
				if (!(position % 8) && (position / 8)) {
					char_address = (0x40 * (position / 8));
					driver->sendCommand(driver->conn_struct, hd44780_SetDDRAMAddr(char_address));
				}
				break;
			case TMBC20464BSP_20x4:
				if (position == 20) {
					line++;
					position = 0;

					switch (line) {
						case 0:
							char_address = 0;
							break;
						case 1:
							char_address = 0x40;
							break;
						case 2:
							char_address = 0x14;
							break;
						case 3:
							char_address = 0x54;
							break;
						default:
							break;
					}

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
		case PVC160101Q_16x1:
			position = position % 16;
			char_address = (position % 8) + (0x40 * (position / 8));
			break;
		case TMBC20464BSP_20x4:
			switch (line) {
				case 0:
					char_address = 0;
					break;
				case 1:
					char_address = 0x40;
					break;
				case 2:
					char_address = 0x14;
					break;
				case 3:
					char_address = 0x54;
					break;
				default:
					break;
			}

			position %= 20;			
			char_address += position;
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

void hd44780_hl_clear(hd44780_driver *driver) {
	if (!driver) return;

	driver->sendCommand(driver->conn_struct, hd44780_ClearDisplay());
}
