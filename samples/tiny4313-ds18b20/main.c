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
	uint8_t buf[9];
	uint8_t count;

	owi_reset(&dsconn);

	owi_searchROM(&dsconn, buffer, &count);

	while (1) {
		owi_skipROM(&dsconn);
		owi_writeByte(&dsconn, 0x44);
		_delay_ms(1000);
		owi_matchROM(&dsconn, buffer);
		owi_readScratchpad(&dsconn, buf, 9);

		fprintf(stdout, "LOOP! %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8]);
		fprintf(stdout, "TMP %u.%04u \n", ((buf[0] >> 4) | ((buf[1]&0x7)<<4)), (buf[0] & 0xF) * 625);
	}

    return 0;
}



