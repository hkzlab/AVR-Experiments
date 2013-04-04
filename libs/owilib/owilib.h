#ifndef _OWILIB_HEADER_
#define _OWILIB_HEADER_

#include <stdint.h>

typedef struct {
	volatile uint8_t *port;
	volatile uint8_t *ddr;
	volatile uint8_t *pin;
	uint8_t pinNum;
} owi_conn;

uint8_t owi_reset(owi_conn *conn);
void owi_readROM(owi_conn *conn, uint8_t buf[8]);

#endif /* _OWILIB_HEADER_ */
