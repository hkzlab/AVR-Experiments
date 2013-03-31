#include "ds1307.h"

#include "twilib.h"

#include <stdlib.h>

#define DS1307_ID 0x0D
#define DS1307_ADDR 0x00

#define DS1307_REG_SEC 0x00
#define DS1307_REG_SQW 0x07
#define DS1307_REG_SRAM 0x08

static I2C_Device ds1307_dev = {DS1307_ID, DS1307_ADDR};

int DS1307_readToD(DS1307_ToD *time) {
	uint8_t clkData[7];
	uint8_t startRegister = DS1307_REG_SEC;

	if (I2C_masterReadRegisterByte(&ds1307_dev, &startRegister, 1, clkData, 7) < 0) return -1; // Read failed...
	
	time->seconds = (clkData[0] & 0x0F) + ((clkData[0] >> 4) * 10);
	time->minutes = (clkData[1] & 0x0F) + ((clkData[1] >> 4) * 10);

	time->hours = (clkData[2] & 0x0F);
	if (clkData[2] & 0x40) { // 12h Day
		time->hours += ((clkData[2] >> 4) & 0x01) * 10;
		time->day12 = 1;
		time->halfDay = ((clkData[2] >> 5) & 0x01);
	} else {
		time->hours += ((clkData[2] >> 4) & 0x03) * 10;
		time->day12 = 0;
	}

	time->dayOfWeek = clkData[3];
	time->dayOfMonth = (clkData[4] & 0x0F) + ((clkData[4] >> 4) * 10);
	time->month = (clkData[5] & 0x0F) + ((clkData[5] >> 4) * 10);
	time->year = 2000 + (clkData[6] & 0x0F) + ((clkData[6] >> 4) * 10);

	return 1;
}

int DS1307_writeToD(DS1307_ToD *time) {
	uint8_t wrData[7];
	uint8_t startRegister = DS1307_REG_SEC;

	wrData[0] = (time->seconds % 10) | ((time->seconds / 10) << 4);
	wrData[1] = (time->minutes % 10) | ((time->minutes / 10) << 4);
	wrData[2] = (time->hours % 10);
	if (time->day12) {
		// 0x40 toggles the 12/24 bit...
		wrData[2] = wrData[2] | 0x40 | (time->halfDay & 0x01) << 5;
		wrData[2] = wrData[2] | ((time->hours / 10) & 0x01) << 4;
	} else {
		wrData[2] = wrData[2] | ((time->hours / 10) & 0x03) << 4;
	}

	wrData[3] = time->dayOfWeek;
	wrData[4] = (time->dayOfMonth % 10) | ((time->dayOfMonth / 10) << 4);
	wrData[5] = (time->month % 10) | ((time->month / 10) << 4);
	uint16_t year = time->year % 100;
	wrData[6] = (year % 10) | ((year / 10) << 4);

	return I2C_masterWriteRegisterByte(&ds1307_dev, &startRegister, 1, wrData, 7);
}

int DS1307_setSQW(uint8_t enable, uint8_t outControl, DS1307_SQWFreq freq) {
	uint8_t wrData = ((outControl << 7) | ((enable & 0x01)  << 4) | freq);
	uint8_t startRegister = DS1307_REG_SQW;

	return I2C_masterWriteRegisterByte(&ds1307_dev, &startRegister, 1, &wrData, 1);
}

int DS1307_writeSRAM(uint8_t *data, uint8_t dLen) {
	uint8_t startRegister = DS1307_REG_SRAM;

	if (dLen > 56) dLen = 56; // We have only 56 bytes...

	return I2C_masterWriteRegisterByte(&ds1307_dev, &startRegister, 1, data, dLen);
}

int DS1307_readSRAM(uint8_t *data, uint8_t dLen) {
	uint8_t startRegister = DS1307_REG_SRAM;

	if (dLen > 56) dLen = 56; // We have only 56 bytes...

	return I2C_masterReadRegisterByte(&ds1307_dev, &startRegister, 1, data, dLen);
}

uint8_t dayOfWeek(uint8_t dayInMonth, uint8_t month, uint16_t year) {
	uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	year -= (month < 3);
	return (year + (year >> 2) - (year/100) + (year/400) + t[month - 1] + dayInMonth) % 7;
}
