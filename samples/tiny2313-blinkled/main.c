#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "twilib.h"
#include "ds1307.h"
#include "shifter_74595.h"

//#include "uart.h"
#include "main.h"

int main(void) {
	// Initialize serial port for output
//	uart_init();
//	stdout = &uart_output;
//	stdin  = &uart_input;

	DDRB |= 0x1B;
	PORTB &= 0xE4;

	shifter_74595_conn *shfConn = shf74595_createConnection(&PORTB, 0, &PORTB, 1, &PORTB, 3, &PORTB, 4);
	shf74595_initConnection(shfConn);

//	I2C_masterBegin(I2C_buildDefaultConfig());

//	DS1307_ToD ToD;
//	DS1307_writeToD(&ToD);

//	DDRD = 0x40;

	uint8_t counter = 0;
	while (1) {
//		PORTD &= 0xBF;
		_delay_ms(1000);
//		PORTD |= 0x40;
//		_delay_ms(1000);

//		DS1307_readToD(&ToD);

//		itoa(ToD.seconds, buffer, 10);
//		buffer[2] = ' ';
//		itoa(ToD.minutes, buffer + 3, 10);
//		buffer[5] = ' ';
//		itoa(ToD.hours, buffer + 6, 10);
				
//		buffer[19] = '\0';

		shf74595_pushData(shfConn, counter, 8);
		shf74595_latchData(shfConn);

	//	for (int i = 0; i < 3; i++)
	//		putchar(buffer[i]);

	//	putchar('\n');

		counter++;
	}

    return 0;
}
