#ifndef _DS1307_HEADER_
#define _DS1307_HEADER_

#include <stdint.h>

typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t halfDay; // AM/PM if 12h day
	uint8_t day12;
	uint8_t dayOfWeek;
	uint8_t dayOfMonth;
	uint8_t month;
	uint16_t year;
} DS1307_ToD;

typedef enum {
	DS1307_SQW_1Hz = 0,
	DS1307_SQW_4096Hz = 1,
	DS1307_SQW_8192Hz = 2,
	DS1307_SQW_32768 = 3
} DS1307_SQWFreq;

void DS1307_readToD(DS1307_ToD *time);
void DS1307_writeToD(DS1307_ToD *time);
void DS1307_setSQW(uint8_t enable, uint8_t outControl, DS1307_SQWFreq freq);

#endif /* _DS1307_HEADER_ */
