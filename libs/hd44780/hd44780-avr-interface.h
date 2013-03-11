#ifndef _HD44780_AVR_INTERFACE_
#define _HD44780_AVR_INTERFACE_

#include <stdint.h>

typedef struct {
	volatile uint8_t *dataPort;
	uint8_t firstDataPin; // 

	volatile uint8_t *rsPort;
	uint8_t rsPin;

	volatile uint8_t *enPort;
	uint8_t enPin;

	volatile uint8_t *rwPort;
	uint8_t rwPin;
} hd44780_connection;

hd44780_connection *hd44780_createConnection(volatile uint8_t *dataPort, uint8_t firstDataPin, volatile uint8_t *rsPort, uint8_t rsPin, volatile uint8_t *enPort, uint8_t enPin, volatile uint8_t *rwPort, uint8_t rwPin);
uint8_t hd44780_initLCD4Bit(hd44780_connection *connection); // returns 4 for 4bit interface!
void hd44780_sendCommand(hd44780_connection *connection, uint16_t command);

#endif /* _HD44780_AVR_INTERFACE_ */
