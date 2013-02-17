#include "hd44780-avr-interface.h"

void hd44780_sendCommand(hd44780_connection *connection, uint16_t command) {
	uint8_t enPortStatus = *(connection->enPort);
	enPortStatus &= (~(1 << connection->enPin));

	if (command & 0x200) {
		;
	} else {
		;
	}

	// Ignore R/W for now

	return;
};
