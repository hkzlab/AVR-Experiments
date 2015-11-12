#include "ad9850.h"

#define ENABLE_PIN(port, num) (port |= (1 << num))
#define DISABLE_PIN(port, num) (port &= (~(1 << num)))

ad9850_conn *ad9850_createConnection(volatile uint8_t *rstPort, uint8_t rstPin, volatile uint8_t *fqupPort, uint8_t fqupPin, volatile uint8_t *wclkPort, uint8_t wclkPin, volatile uint8_t *dataPort, uint8_t dataPin) {
	ad9850_conn *conn = (ad9850_conn*)malloc(sizeof(ad9850_conn));

	conn->rstPort = rstPort;
	conn->rstPin = rstPin;

	conn->fqupPort = fqupPort;
	conn->fqupPin = fqupPin;

	conn->wclkPort = wclkPort;
	conn->wclkPin = wclkPin;

	conn->dataPort = dataPort;
	conn->dataPin = dataPin;

	return conn;
}

void ad9850_initConnection(ad9850_conn *conn) {
	DISABLE_PIN(*conn->fqupPort, conn->fqupPin);
	DISABLE_PIN(*conn->wclkPort, conn->wclkPin);
	DISABLE_PIN(*conn->dataPort, conn->dataPin);
	
	ENABLE_PIN(*conn->rstPort, conn->rstPin); // Keep the module in RESET
}

void ad9850_setReset(ad9850_conn *conn, uint8_t reset) {
	if (reset) ENABLE_PIN(*conn->rstPort, conn->rstPin);
	else DISABLE_PIN(*conn->rstPort, conn->rstPin); // Keep the module in RESET
}

