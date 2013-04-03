#ifndef _DS18B20_HEADER_
#define _DS18B20_HEADER_

#include <stdint.h>

typedef struct {
	volatile uint8_t *port;
	volatile uint8_t *ddr;
	volatile uint8_t *pin;
	uint8_t pinNum;
} ds18b20_conn;

uint8_t ds18b20_reset(ds18b20_conn *conn);

void ds18b20_readROM(ds18b20_conn *conn, uint8_t buf[8]);

#endif /* _DS18B20_HEADER_ */
