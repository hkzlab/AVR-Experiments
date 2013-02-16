/*
 * Test how to use uart stuff as an outside library. Redirects stdout
 * to uart.
 * 
 * Led at Arduino digital 13 (PORTB5). Cause we all love blinken leds!
 *
 * To compile and upload run: make clean; make; make program;
 * Connect to serial with: screen /dev/tty.usbserial-* 9600
 *
 * Copyright 2011 Mika Tuupola
 *
 * Licensed under the MIT license:
 *   http://www.opensource.org/licenses/mit-license.php
 *
 */
 
#ifndef F_CPU
#define F_CPU 8000000UL // MCU Frequency set to 8Mhz
#endif

#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "main.h"
#include "uart.h"

int main(void) {    

	// Initialize serial port for output
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

/*
	// Set port A and port C of the MCU as output
	DDRA = 0xFF;
	DDRC = 0xFF;

	// Start with address 0
	PORTA = 0x00;
	PORTC = 0x00;

	_delay_ms(6000);
*/
	
	while (1) {
		fprintf(stdout, "Loop!");
		//PORTA = (address >> 8) & 0x00FF;
		_delay_ms(5);
	}
   
    return 0;
}
