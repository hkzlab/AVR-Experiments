#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "main.h"


int main(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	printf("INIT!\n");

	DDRA = 0xFF;
	PORTA = 0xFE;

	while(1) {
		PORTA = ~PORTA;	
		_delay_ms(1000);
		printf("loop\n");
	}

	return 0;
}


