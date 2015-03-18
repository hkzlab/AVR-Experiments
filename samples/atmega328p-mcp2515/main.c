#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "spi.h"

#include "uart.h"
#include "main.h"

static volatile uint8_t interrupt_received = 0;

int main(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	while (1) {
		_delay_ms(1000);		
		fprintf(stdout, "nope\n");
	}

    return 0;
}



