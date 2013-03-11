#ifndef _HD44780_74595_INTERFACE_
#define _HD44780_74595_INTERFACE_

#include <stdint.h>
#include "shifter_74595.h"

typedef struct {
	shifter_74595_conn *shfConn;
	uint8_t firstDataPin; 
	uint8_t rsPin;
	uint8_t enPin;

	uint8_t ioStat;
} hd44780_74595_connection;

hd44780_74595_connection *hd44780_74595_createConnection(shifter_74595_conn* shfConn, uint8_t firstDataPin, uint8_t rsPin, uint8_t enPin);
uint8_t hd44780_74595_initLCD4Bit(hd44780_74595_connection *connection); // returns 4 for 4bit interface!
void hd44780_74595_sendCommand(hd44780_74595_connection *connection, uint16_t command);


#endif /* _HD44780_74595_INTERFACE_ */
