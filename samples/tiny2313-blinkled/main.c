#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "twilib.h"
#include "ds1307.h"

#include "uart.h"
#include "main.h"

static char buffer[20];

int main(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	I2C_masterBegin(I2C_buildDefaultConfig());

	DS1307_ToD ToD;

	DDRD = 0x40;
	while (1) {
		PORTD &= 0xBF;
		_delay_ms(1000);
		PORTD |= 0x40;
		_delay_ms(1000);

		DS1307_readToD(&ToD);

		itoa(ToD.seconds, buffer, 10);
		buffer[2] = ' ';
		itoa(ToD.minutes, buffer + 3, 10);
		buffer[5] = ' ';
		itoa(ToD.hours, buffer + 6, 10);
				
		buffer[19] = '\0';

		for (int i = 0; i < 20; i++)
			putchar(buffer[i]);

		putchar('\n');
	}

    return 0;
}
