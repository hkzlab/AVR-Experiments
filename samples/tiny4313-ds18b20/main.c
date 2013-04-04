#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "owilib.h"

#include "uart.h"
#include "main.h"

static volatile uint8_t interrupt_received = 0;

int main(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	DDRB = 0xFF;
	PORTB = 0x00;

	owi_conn dsconn;

	dsconn.port = &PORTB;
	dsconn.pin = &PINB;
	dsconn.ddr = &DDRB;
	dsconn.pinNum = 0;

	fprintf(stdout, "AVVIO\n");

	uint8_t buffer[16];
	uint8_t count;

	owi_reset(&dsconn);

	owi_searchROM(&dsconn, buffer, &count);

	while (1) {
		_delay_ms(1000);
		fprintf(stdout, "LOOP! %d %.2X %.2X\n", count, buffer[0], buffer[1]);
	}

    return 0;
}



