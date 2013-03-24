#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "main.h"
#include "uart.h"

#include "shifter_74595.h"

int main(void) {
	// Initialize serial port for output
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	DDRD = 0xFC;
	DDRB = 0xFF;
	
	PORTD &= 0x03;
	PORTB = 0x00;

	shifter_74595_conn *shfConn = shf74595_createConnection(&PORTD, 5, &PORTD, 7, &PORTB, 0, &PORTB, 1);
	shf74595_initConnection(shfConn);

	uint8_t counter = 0;
	while (1) {
		shf74595_pushData(shfConn, counter, 8);
		shf74595_latchData(shfConn);
	//	_delay_ms(50);

		counter++;
	}

    return 0;
}
