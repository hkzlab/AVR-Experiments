 
#ifndef F_CPU
#define F_CPU 8000000UL // MCU Frequency set to 8Mhz
#endif

#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <util/delay.h>

#include "main.h"
#include "uart.h"

#include "twilib.h"
#include "ds1307.h"

volatile uint8_t interruptReceived = 0;

int main(void) {
	// Initialize serial port for output
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	DS1307_ToD time;

	I2C_Config *masterConfig = I2C_buildDefaultConfig();
	I2C_masterBegin(masterConfig);

	DS1307_setSQW(1, 0, DS1307_SQW_1Hz);
	DS1307_readToD(&time);

	DDRD = DDRD & 0xCF;
	// See here for interrupts: http://www.protostack.com/blog/2010/09/external-interrupts-on-an-atmega168/
	EIMSK |= 1 << INT1;
	EICRA |= 1 << ISC11;
	sei();

	_delay_ms(500);
	fprintf(stdout, "Start loop...\n");	

	while (1) {
		if (interruptReceived) {
			interruptReceived = 0;
			DS1307_readToD(&time);
			fprintf(stdout, "%.2u-%.2u-%.4u %.2u:%.2u:%.2u\n", time.dayOfMonth, time.month, time.year, time.hours, time.minutes, time.seconds);
		}
	}

    return 0;
}

ISR(INT1_vect) {
	interruptReceived = 1;
}
