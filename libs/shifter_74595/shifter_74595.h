#ifndef _SHIFTER_74595_HEADER_
#define _SHIFTER_74595_HEADER_

#include <stdint.h>

typedef struct {
	volatile uint8_t *inpPort;
	uint8_t inpPin; // 

	volatile uint8_t *latchPort;
	uint8_t latchPin;

	volatile uint8_t *clkPort;
	uint8_t clkPin;

	volatile uint8_t *clrPort;
	uint8_t clrPin;
} shifter_74595_conn;

shifter_74595_conn *shf74595_createConnection(volatile uint8_t *inpPort, uint8_t inpPin, volatile uint8_t *latchPort, uint8_t latchPin, volatile uint8_t *clkPort, uint8_t clkPin, volatile uint8_t *clrPort, uint8_t clrPin);
void shf74595_initConnection(shifter_74595_conn *conn);
void shf74595_pushData(shifter_74595_conn *conn, uint8_t data, uint8_t count);
void shf74595_latchData(shifter_74595_conn *conn);
void shf74595_clear(shifter_74595_conn *conn);


#endif /* _SHIFTER_74595_HEADER_ */
