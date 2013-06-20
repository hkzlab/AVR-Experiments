#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "ps2_keyb.h"

#include "main.h"

int main(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	printf("INIT!\n");
	ps2keyb_init(&PORTD, &DDRD, &PIND, 3);

	sei();

	 while(1);

    return 0;
}


