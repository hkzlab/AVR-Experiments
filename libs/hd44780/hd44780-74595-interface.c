#include "hd44780-74595-interface.h"

#include <stdlib.h>
#include <util/delay.h>

#define ENABLE_PIN(port, num) (port |= (1 << num))
#define DISABLE_PIN(port, num) (port &= (~(1 << num)))

hd44780_74595_connection *hd44780_74595_createConnection(shifter_74595_conn* shfConn, uint8_t firstDataPin, uint8_t rsPin, uint8_t enPin) {
	hd44780_74595_connection *conn = (hd44780_74595_connection*)malloc(sizeof(hd44780_74595_connection));

	conn->shfConn = shfConn;

	conn->firstDataPin = firstDataPin;
	conn->rsPin = rsPin;
	conn->enPin = enPin;

	conn->ioStat = 0;

	return conn;
}

uint8_t hd44780_74595_initLCD4Bit(hd44780_74595_connection *connection) {
	shf74595_clear(connection->shfConn);

	connection->ioStat = 0;
	shf74595_pushData(connection->shfConn, connection->ioStat, 8);
	shf74595_latchData(connection->shfConn);

	uint8_t dataMask = ~(0xF << connection->firstDataPin);	

	for (uint8_t loop = 0; loop < 3; loop++) {
		connection->ioStat &= dataMask;
		connection->ioStat |= (0x03 << connection->firstDataPin);
		shf74595_pushData(connection->shfConn, connection->ioStat, 8);
		shf74595_latchData(connection->shfConn);

		ENABLE_PIN(connection->ioStat, connection->enPin);
		shf74595_pushData(connection->shfConn, connection->ioStat, 8);
		shf74595_latchData(connection->shfConn);
		_delay_ms(6);

		DISABLE_PIN(connection->ioStat, connection->enPin);
		shf74595_pushData(connection->shfConn, connection->ioStat, 8);
		shf74595_latchData(connection->shfConn);
		_delay_ms(6);				
	}

	connection->ioStat &= dataMask;
	connection->ioStat |= (0x02 << connection->firstDataPin);
	shf74595_pushData(connection->shfConn, connection->ioStat, 8);
	shf74595_latchData(connection->shfConn);

	ENABLE_PIN(connection->ioStat, connection->enPin);
	shf74595_pushData(connection->shfConn, connection->ioStat, 8);
	shf74595_latchData(connection->shfConn);
	_delay_ms(6);

	DISABLE_PIN(connection->ioStat, connection->enPin);
	shf74595_pushData(connection->shfConn, connection->ioStat, 8);
	shf74595_latchData(connection->shfConn);
	_delay_ms(6);	

	return 4;
}

void hd44780_74595_sendCommand(hd44780_74595_connection *connection, uint16_t command) {
	uint8_t dataMask = ~(0xF << connection->firstDataPin);	
	uint8_t commandByte = command & 0xFF;

	DISABLE_PIN(connection->ioStat, connection->enPin);
	shf74595_pushData(connection->shfConn, connection->ioStat, 8);
	shf74595_latchData(connection->shfConn);

	if (command & 0x200) // Set the RS pin
		ENABLE_PIN(connection->ioStat, connection->rsPin);
	else
		DISABLE_PIN(connection->ioStat, connection->rsPin);
	shf74595_pushData(connection->shfConn, connection->ioStat, 8);
	shf74595_latchData(connection->shfConn);

	// Put the HIGH nibble of command on data lines
	connection->ioStat &= dataMask;
	connection->ioStat |= (((commandByte >> 4) & 0xF) << connection->firstDataPin);

	shf74595_pushData(connection->shfConn, connection->ioStat, 8);
	shf74595_latchData(connection->shfConn);

	ENABLE_PIN(connection->ioStat, connection->enPin);
	shf74595_pushData(connection->shfConn, connection->ioStat, 8);
	shf74595_latchData(connection->shfConn);
	_delay_ms(1);

	DISABLE_PIN(connection->ioStat, connection->enPin);
	shf74595_pushData(connection->shfConn, connection->ioStat, 8);
	shf74595_latchData(connection->shfConn);
	_delay_ms(1);	

	// Put the LOW nibble of command on data lines
	connection->ioStat &= dataMask;
	connection->ioStat |= ((commandByte & 0xF) << connection->firstDataPin);
	shf74595_pushData(connection->shfConn, connection->ioStat, 8);
	shf74595_latchData(connection->shfConn);

	// Pull EN high...
	ENABLE_PIN(connection->ioStat, connection->enPin);
	shf74595_pushData(connection->shfConn, connection->ioStat, 8);
	shf74595_latchData(connection->shfConn);
	_delay_ms(1);

	DISABLE_PIN(connection->ioStat, connection->enPin);
	shf74595_pushData(connection->shfConn, connection->ioStat, 8);
	shf74595_latchData(connection->shfConn);
	_delay_ms(5);	

	return;
}
