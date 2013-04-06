#ifndef _DS18B20_HEADER_
#define _DS18B20_HEADER_

#include <stdint.h>
#include "owilib.h"

typedef enum {
	DS_THRM_ERROR = 4,
	DS_THRM_9BIT = 0,
	DS_THRM_10BIT = 1,
	DS_THRM_11BIT = 2,
	DS_THRM_12BIT = 3
} ds18b20_thrm_cfg;

typedef struct {
	ds18b20_thrm_cfg thrmcfg;
	int8_t lT, hT;
} ds18b20_cfg;

uint8_t ds18b20_checkROMValidity(uint8_t addr[8]);
uint8_t *ds18b20_buildFullROMAddress(uint8_t shortAddr[6]);
ds18b20_cfg ds18b20_getCFG(owi_conn *conn, uint8_t addr[8]);
void ds18b20_setCFG(owi_conn *conn, uint8_t addr[8], ds18b20_cfg *cfg);
void ds18b20_startTempConversion(owi_conn *conn, uint8_t addr[8]);
uint8_t ds18b20_getTemp(owi_conn *conn, uint8_t addr[8], int8_t *integer, uint16_t *decimal);

#endif /* _DS18B20_HEADER_ */
