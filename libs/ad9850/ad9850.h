#ifndef __AD9850_HEADER__
#define __AD9850_HEADER__

typedef struct {
	volatile uint8_t *rstPort;
	uint8_t rstPin; 

	volatile uint8_t *fqupPort;
	uint8_t fqupPin;

	volatile uint8_t *wclkPort;
	uint8_t wclkPin;

	volatile uint8_t *dataPort;
	uint8_t dataPin;
} ad9850_conn;

ad9850_conn *ad9850_createConnection(volatile uint8_t *rstPort, uint8_t rstPin, volatile uint8_t *fqupPort, uint8_t fqupPin, volatile uint8_t *wclkPort, uint8_t wclkPin, volatile uint8_t *dataPort, uint8_t dataPin);
void ad9850_initConnection(ad9850_conn *conn);

void ad9850_setReset(ad9850_conn *conn, uint8_t reset);
#endif /* __AD9850_HEADER__ */
