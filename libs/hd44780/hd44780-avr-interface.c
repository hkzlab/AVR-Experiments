#include "hd44780-avr-interface.h"

#include <stdlib.h>

#include <util/delay.h>

#define ENABLE_PIN(port, num) (port |= (1 << num))
#define DISABLE_PIN(port, num) (port &= (~(1 << num)))

hd44780_connection *hd44780_createConnection(volatile uint8_t *dataPort, uint8_t firstDataPin, volatile uint8_t *rsPort, uint8_t rsPin, volatile uint8_t *enPort, uint8_t enPin, volatile uint8_t *rwPort, uint8_t rwPin) {
	hd44780_connection *conn = (hd44780_connection*)malloc(sizeof(hd44780_connection));

	conn->dataPort = dataPort;
	conn->firstDataPin = firstDataPin;

	conn->rsPort = rsPort;
	conn->rsPin = rsPin;

	conn->enPort = enPort;
	conn->enPin = enPin;

	conn->rwPort = rwPort;
	conn->rwPin = rwPin;

	return conn;
}

uint8_t hd44780_initLCD4Bit(hd44780_connection *connection) {
	uint8_t enPortStatus, rsPortStatus, dataPortStatus;

	// Pull EN high...	
	enPortStatus = *(connection->enPort);
	DISABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus; // Lower the EN line!
	
	// Put EN low again...
	rsPortStatus = *(connection->enPort);
	DISABLE_PIN(rsPortStatus, connection->rsPin);
	*(connection->rsPort) = rsPortStatus; // Disable the RS port!

	uint8_t dataMask = ~(0xF << connection->firstDataPin);

	for (uint8_t loop = 0; loop < 3; loop++) {
		dataPortStatus = *(connection->dataPort);
		dataPortStatus &= dataMask;
		dataPortStatus |= (0x03 << connection->firstDataPin);
		*(connection->dataPort) = dataPortStatus; 

		// Pull EN high...
		enPortStatus = *(connection->enPort);
		ENABLE_PIN(enPortStatus, connection->enPin);
		*(connection->enPort) = enPortStatus;
		_delay_ms(6);

		// Put EN low again...
		enPortStatus = *(connection->enPort);
		DISABLE_PIN(enPortStatus, connection->enPin);
		*(connection->enPort) = enPortStatus; 
		_delay_ms(6);
	}

	dataPortStatus = *(connection->dataPort);
	dataPortStatus &= dataMask;
	dataPortStatus |= (0x02 << connection->firstDataPin);
	*(connection->dataPort) = dataPortStatus; 		

	// Pull EN high...
	enPortStatus = *(connection->enPort);
	ENABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus; 
	_delay_ms(6);

	// Put EN low again...
	enPortStatus = *(connection->enPort);
	DISABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus;
	_delay_ms(6);

	return 4;
}

void hd44780_sendCommand(hd44780_connection *connection, uint16_t command) {
	uint8_t enPortStatus, rsPortStatus, dataPortStatus;
//	uint8_t rwPortStatus = *(connection->rwPort); // Ignore RW line & commands for now

	uint8_t dataMask = ~(0xF << connection->firstDataPin);	
	uint8_t commandByte = command & 0xFF;

	// Put EN low again...	
	enPortStatus = *(connection->enPort);
	DISABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus; // Lower the EN line!

	rsPortStatus = *(connection->rsPort);
	if (command & 0x200) // Set the RS pin
		ENABLE_PIN(rsPortStatus, connection->rsPin);
	else
		DISABLE_PIN(rsPortStatus, connection->rsPin);
	*(connection->rsPort) = rsPortStatus; // set the RS port!

	// Put the HIGH nibble of command on data lines
	dataPortStatus = *(connection->dataPort);
	dataPortStatus &= dataMask;
	dataPortStatus |= (((commandByte >> 4) & 0xF) << connection->firstDataPin);
	*(connection->dataPort) = dataPortStatus; 

	// Pull EN high...
	enPortStatus = *(connection->enPort);
	ENABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus; // Enable the EN line!
	_delay_ms(1);

	// Put EN low again...
	enPortStatus = *(connection->enPort);
	DISABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus;
	_delay_ms(1);	

	// Put the LOW nibble of command on data lines
	dataPortStatus = *(connection->dataPort);
	dataPortStatus &= dataMask;
	dataPortStatus |= ((commandByte & 0xF) << connection->firstDataPin);
	*(connection->dataPort) = dataPortStatus; 

	// Pull EN high...
	enPortStatus = *(connection->enPort);
	ENABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus;
	_delay_ms(1);

	// Put EN low again...
	enPortStatus = *(connection->enPort);
	DISABLE_PIN(enPortStatus, connection->enPin);
	*(connection->enPort) = enPortStatus; 
	_delay_ms(5);	

	return;
};
