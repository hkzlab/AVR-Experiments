#include "ds18b20.h"
#include <stdlib.h>

#define DS18B20_DEVCODE 0x28

typedef enum {
	ds_startTempConversion = 0x44,
	ds_recallEE = 0xB8,
	ds_writeEE = 0x48
} ds18b20_cmd;

static uint16_t thrm_decimal_steps[] = {5, 25, 125, 625}; // Taken from datasheet

void ds18b20_writeEEPROM(owi_conn *conn, uint8_t addr[8]) {
	owi_reset(conn);
	if (addr) {
		owi_matchROM(conn, addr);
	} else {
		owi_skipROM(conn);
	}

	owi_writeByte(conn, ds_writeEE);
	owi_reset(conn);
}

void ds18b20_readEEPROM(owi_conn *conn, uint8_t addr[8]) {
	owi_reset(conn);
	if (addr) {
		owi_matchROM(conn, addr);
	} else {
		owi_skipROM(conn);
	}

	owi_writeByte(conn, ds_recallEE);
	owi_reset(conn);
}

uint8_t ds18b20_checkROMValidity(uint8_t addr[8]) {
	return (owi_calcCRC(addr, 7) == addr[7]);
}

uint8_t *ds18b20_buildFullROMAddress(uint8_t shortAddr[6]) {
	uint8_t *addr = (uint8_t*)malloc(8);
	addr[0] = DS18B20_DEVCODE;
	for (uint8_t idx = 0; idx < 6; idx++)
		addr[idx + 1] = shortAddr[idx];

	addr[7] = owi_calcCRC(addr, 7);

	return addr;
}

ds18b20_cfg *ds18b20_getCFG(owi_conn *conn, uint8_t addr[8]) {
	uint8_t buf[9];
	ds18b20_cfg *cfg = (ds18b20_cfg*)malloc(sizeof(ds18b20_cfg));
	
	cfg->thrmcfg = DS_THRM_ERROR;
	cfg->lT = cfg->hT = 0;

	owi_reset(conn);	
	owi_matchROM(conn, addr);
	owi_readScratchpad(conn, buf, 9);

	if (owi_calcCRC(buf, 8) != buf[8]) return cfg; // CRC error! Communication problem

	cfg->thrmcfg = (buf[4] >> 5) & 0x03;
	cfg->hT = buf[2];
	cfg->lT = buf[3];

	return cfg;
}

void ds18b20_setCFG(owi_conn *conn, uint8_t addr[8], ds18b20_cfg *cfg) {
	uint8_t buf[3];

	buf[2] = 0x1F | (cfg->thrmcfg << 5);
	buf[1] = cfg->hT;
	buf[0] = cfg->lT;

	owi_reset(conn);
	if (addr) {
		owi_matchROM(conn, addr);
	} else {
		owi_skipROM(conn);
	}

	owi_writeScratchpad(conn, buf, 3);
	owi_reset(conn);
	
}

void ds18b20_startTempConversion(owi_conn *conn, uint8_t addr[8]) {
	owi_reset(conn);
	if (addr) {
		owi_matchROM(conn, addr);
	} else {
		owi_skipROM(conn);
	}

	owi_writeByte(conn, ds_startTempConversion);
	owi_reset(conn);
}

uint8_t ds18b20_getTemp(owi_conn *conn, uint8_t addr[8], int8_t *integer, uint16_t *decimal) {
	uint8_t buf[9];
	ds18b20_cfg *cfg = ds18b20_getCFG(conn, addr);

	if (cfg->thrmcfg == DS_THRM_ERROR) {
		free(cfg);
		return 0;
	}

	owi_matchROM(conn, addr);
	owi_readScratchpad(conn, buf, 9);

	if (owi_calcCRC(buf, 8) != buf[8]) {
		free(cfg);
		return 0; // Communication problem
	}

	*integer = 0;
	*decimal = 0;

	uint8_t mask = 0, shft = 0;
	switch (cfg->thrmcfg) {
		case DS_THRM_12BIT:
			mask = 0x0F;
			shft = 0;
			break;
		case DS_THRM_11BIT:
			mask = 0x0E;
			shft = 1;
			break;
		case DS_THRM_10BIT:
			mask = 0x0C;
			shft = 2;
			break;
		case DS_THRM_9BIT:
			mask = 0x08;
			shft = 3;
			break;
		default:
			break;
	}

	*integer = (buf[0] >> 4) | ((buf[1] & 0x7) << 4);
	*decimal = (buf[0] & mask) >> shft; 
	*decimal *= thrm_decimal_steps[cfg->thrmcfg];

	free(cfg);

	return 1;
}
