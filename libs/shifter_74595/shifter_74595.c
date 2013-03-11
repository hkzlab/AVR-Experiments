#include "shifter_74595.h"

#include <stdlib.h>
#include <util/delay.h>

#define ENABLE_PIN(port, num) (port |= (1 << num))
#define DISABLE_PIN(port, num) (port &= (~(1 << num)))

shifter_74595_conn *shf74595_createConnection(volatile uint8_t *inpPort, uint8_t inpPin, volatile uint8_t *latchPort, uint8_t latchPin, volatile uint8_t *clkPort, uint8_t clkPin, volatile uint8_t *clrPort, uint8_t clrPin) {
	shifter_74595_conn *conn = (shifter_74595_conn*)malloc(sizeof(shifter_74595_conn));

	conn->inpPort = inpPort;
	conn->inpPin = inpPin;

	conn->latchPort = latchPort;
	conn->latchPin = latchPin;

	conn->clkPort = clkPort;
	conn->clkPin = clkPin;

	conn->clrPort = clrPort;
	conn->clrPin = clrPin;

	return conn;
}

void shf74595_initConnection(shifter_74595_conn *conn) {
	DISABLE_PIN(*conn->inpPort, conn->inpPin);
	DISABLE_PIN(*conn->latchPort, conn->latchPin);
	DISABLE_PIN(*conn->clkPort, conn->clkPin);

	DISABLE_PIN(*conn->clrPort, conn->clrPin);
	//asm("nop");
	ENABLE_PIN(*conn->clrPort, conn->clrPin);			
}

void shf74595_pushData(shifter_74595_conn *conn, uint8_t data, uint8_t count) {
	while (count--) {
		if (data & 0x80)
			ENABLE_PIN(*conn->inpPort, conn->inpPin);
		else
			DISABLE_PIN(*conn->inpPort, conn->inpPin);

		ENABLE_PIN(*conn->clkPort, conn->clkPin);
	//	asm("nop");	
		DISABLE_PIN(*conn->clkPort, conn->clkPin);
	//	asm("nop");
		
		data <<= 1;
	}
}

void shf74595_latchData(shifter_74595_conn *conn) {
		ENABLE_PIN(*conn->latchPort, conn->latchPin);
	//	asm("nop");
		DISABLE_PIN(*conn->latchPort, conn->latchPin);
	//	asm("nop");
}

void shf74595_clear(shifter_74595_conn *conn) {
		DISABLE_PIN(*conn->clrPort, conn->clrPin);
//		asm("nop");
		ENABLE_PIN(*conn->clrPort, conn->clrPin);
//		asm("nop");
}
