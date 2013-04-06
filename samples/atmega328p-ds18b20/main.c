#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "owilib.h"
#include "ds18b20.h"

#include "uart.h"
#include "main.h"

static volatile uint8_t interrupt_received = 0;

int main(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	DDRD = 0xFF;
	PORTD = 0x00;

	owi_conn dsconn;

	dsconn.port = &PORTD;
	dsconn.pin = &PIND;
	dsconn.ddr = &DDRD;
	dsconn.pinNum = 7;

	fprintf(stdout, "AVVIO\n");

	uint8_t buffer[16];
	uint8_t count;

	owi_reset(&dsconn);
	owi_searchROM(&dsconn, buffer, &count, 0);

	int8_t integ;
	uint16_t decim;

	while (1) {
		ds18b20_startTempConversion(&dsconn, NULL);
		_delay_ms(1000);		
		ds18b20_getTemp(&dsconn, buffer, &integ, &decim);
		fprintf(stdout, "%d.%04u\n", integ, decim);
	}

    return 0;
}



