#ifndef _OWILIB_HEADER_
#define _OWILIB_HEADER_

#include <stdint.h>

typedef struct {
	volatile uint8_t *port;
	volatile uint8_t *ddr;
	volatile uint8_t *pin;
	uint8_t pinNum;
} owi_conn;

uint8_t owi_readByte(owi_conn *conn);
void owi_writeByte(owi_conn *conn, uint8_t data);

uint8_t owi_reset(owi_conn *conn);
void owi_readROM(owi_conn *conn, uint8_t buf[8]);
void owi_searchROM(owi_conn *conn, uint8_t *buf, uint8_t *count);
void owi_matchROM(owi_conn *conn, uint8_t rom[8]);
void owi_skipROM(owi_conn *conn);
void owi_readScratchpad(owi_conn *conn, uint8_t *buf, uint16_t len);
void owi_writeScratchpad(owi_conn *conn, uint8_t *buf, uint16_t len);
void owi_copyScratchpad(owi_conn *conn);

uint8_t owi_calcCRC(uint8_t *data, uint16_t len);


#endif /* _OWILIB_HEADER_ */
