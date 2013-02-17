#include "hd44780-avr-interface.h"

#include <util/delay.h>

#define ENABLE_PIN(port, num) (port |= (1 << num))
#define DISABLE_PIN(port, num) (port &= (~(1 << num)))

void hd44780_initLCD4Bit(hd44780_connection *connection) {
	uint8_t enPortStatus, rsPortStatus, dataPortStatus;

	enPortStatus = *(connection->enPort);
	DISABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus; // Lower the EN line!
	
	rsPortStatus = *(connection->enPort);
	DISABLE_PIN(rsPortStatus, connection->rsPin);
	*(connection->rsPort) = rsPortStatus; // Disable the RS port!

	for (uint8_t loop = 0; loop < 3; loop++) {
		dataPortStatus = *(connection->dataPort);
		dataPortStatus &= (0xF0 >> (connection->dataPinsBlock * 4));
		dataPortStatus |= (0x03 << (connection->dataPinsBlock * 4));
		*(connection->dataPort) = dataPortStatus; 

		// Pull EN high...
		enPortStatus = *(connection->enPort);
		ENABLE_PIN(enPortStatus, connection->enPin);
		*(connection->enPort) = enPortStatus; // Put hight the EN line!
		_delay_ms(10);

		// Put EN low again...
		enPortStatus = *(connection->enPort);
		DISABLE_PIN(enPortStatus, connection->enPin);
		*(connection->enPort) = enPortStatus; // Lower the EN line!
		_delay_ms(10);
	}

	dataPortStatus = *(connection->dataPort);
	dataPortStatus &= (0xF0 >> (connection->dataPinsBlock * 4));
	dataPortStatus |= (0x02 << (connection->dataPinsBlock * 4));
	*(connection->dataPort) = dataPortStatus; 		

	// Pull EN high...
	enPortStatus = *(connection->enPort);
	ENABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus; // Lower the EN line!
	_delay_ms(10);

	// Put EN low again...
	enPortStatus = *(connection->enPort);
	DISABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus; // Lower the EN line!
	_delay_ms(10);
}

void hd44780_sendCommand(hd44780_connection *connection, uint16_t command) {
	uint8_t enPortStatus, rsPortStatus, dataPortStatus;
//	uint8_t rwPortStatus = *(connection->rwPort);

	uint8_t commandByte = command & 0xFF;

	enPortStatus = *(connection->enPort);
	DISABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus; // Lower the EN line!

	// Ignore R/W for now...	

	rsPortStatus = *(connection->rsPort);
	if (command & 0x200) // Set the RS pin
		ENABLE_PIN(rsPortStatus, connection->rsPin);
	else
		DISABLE_PIN(rsPortStatus, connection->rsPin);
	*(connection->rsPort) = rsPortStatus; // set the RS port!

	// Put the HIGH byte of command on data lines
	dataPortStatus = *(connection->dataPort);
	dataPortStatus &= (0xF0 >> (connection->dataPinsBlock * 4));
	dataPortStatus |= (((commandByte >> 4) & 0xF) << (connection->dataPinsBlock * 4));
	*(connection->dataPort) = dataPortStatus; 

	// Pull EN high...
	enPortStatus = *(connection->enPort);
	ENABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus; // Enable the EN line!
	_delay_ms(10);

	// Put EN low again...
	enPortStatus = *(connection->enPort);
	DISABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus; // Lower the EN line!
	_delay_ms(10);	

	// Put the LOW byte of command on data lines
	dataPortStatus = *(connection->dataPort);
	dataPortStatus &= (0xF0 >> (connection->dataPinsBlock * 4));
	dataPortStatus |= ((commandByte & 0xF) << (connection->dataPinsBlock * 4));
	*(connection->dataPort) = dataPortStatus; 

	// Pull EN high...
	enPortStatus = *(connection->enPort);
	ENABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus; // Enable the EN line!
	_delay_ms(10);

	// Put EN low again...
	enPortStatus = *(connection->enPort);
	DISABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus; // Lower the EN line!
	_delay_ms(10);	

	return;
};
