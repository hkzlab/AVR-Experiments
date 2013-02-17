#include <stdlib.h>

typedef struct {
	uint8_t *dataPort;
	uint8_t dataPins[4];

	uint8_t *rsPort;
	uint8_t rsPin;

	uint8_t *enPort;
	uint8_t enPin;

	uint8_t *rwPort;
	uint8_t rwPin;
} hd44780_connection;

void hd44780_sendCommand(hd44780_connection *connection, uint16_t command);

