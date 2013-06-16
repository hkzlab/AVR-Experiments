#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "twilib.h"
#include "ds1307.h"
#include "shifter_74595.h"

#include "uart.h"
#include "main.h"

static volatile uint8_t interrupt_received = 0;

int main(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	DDRB |= 0x1B;
	PORTB &= 0xE4;

	shifter_74595_conn *shfConn = shf74595_createConnection(&PORTB, 0, &PORTB, 1, &PORTB, 3, &PORTB, 4);
	shf74595_initConnection(shfConn);
	shf74595_clear(shfConn);

	I2C_masterBegin(I2C_buildDefaultConfig());

	DS1307_ToD ToD;

/*
	ToD.seconds = 0;
	ToD.minutes = 21;
	ToD.hours = 16;
	ToD.dayOfMonth = 24;
	ToD.month = 3;
	ToD.dayOfWeek = 7;
	DS1307_writeToD(&ToD);
*/
	DS1307_setSQW(1, 0, DS1307_SQW_1Hz);	

	DDRD = 0x40;
	PORTD |= 0x40;


	fprintf(stdout, "AVVIO\n");

	PCMSK |= (1 << PIND2);
	MCUCR |= ((1 << ISC00) | (1 << ISC01));
	GIMSK |= (1 << INT0);
	sei();

	while (1) {
		if (interrupt_received) {
			interrupt_received = 0;

			DS1307_readToD(&ToD);			

			PORTD ^= 0x40;
			shf74595_pushData(shfConn, ToD.minutes, 8);
			shf74595_latchData(shfConn);
		}


	}

    return 0;
}

ISR(INT0_vect) {
	interrupt_received = 1;
}


