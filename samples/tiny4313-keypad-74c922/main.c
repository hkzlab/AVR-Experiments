#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "main.h"

static volatile uint8_t interrupt_received = 0;

int main(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	DDRB &= 0xE0;
	PORTB &= 0xE0;

	PCMSK |= (1 << PCINT4);
	GIMSK |= (1 << PCIE);
	sei();

	uint8_t key, nKey;

	key = 0xFF;
	while (1) {
		if (interrupt_received) {
			interrupt_received = 0;
		
			nKey = (PINB & 0x0F);
			nKey = (nKey - (nKey/4));

			if (nKey == key) {
				key = 0xFF;
				fprintf(stdout, "%.2X - release\n", nKey);
			} else {
				key = nKey;
				fprintf(stdout, "%.2X - press\n", nKey);
			}
		}


	}

    return 0;
}

ISR(PCINT_B_vect) {
	interrupt_received = 1;
}



