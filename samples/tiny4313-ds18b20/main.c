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

	owi_reset(&dsconn);

	while (1) {
		owi_readROM(&dsconn, buffer);
		fprintf(stdout, "val %.02X %.02X %.02X %.02X %.02X %.02X %.02X %.02X\n",  buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
		_delay_ms(1000);
	}

    return 0;
}



