#include "ds1307.h"

#include "twilib.h"

#include <stdlib.h>

#define DS1307_ID 0x0D
#define DS1307_ADDR 0x00

#define DS1307_REG_SEC 0x00
#define DS1307_REG_SQW = 0x07

static I2C_Device ds1307_dev = {DS1307_ID, DS1307_ADDR};

void DS1307_readToD(DS1307_ToD *time) {
	uint8_t clkData[7];
	uint8_t startRegister = DS1307_REG_SEC;

	if (I2C_masterReadRegisterByte(&ds1307_dev, &startRegister, 1, clkData, 7) < 0) return; // Read failed...
	
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
}
