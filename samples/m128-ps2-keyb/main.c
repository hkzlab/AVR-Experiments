#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "ps2_keyb.h"
#include "ps2_proto.h"

#include "amiga_keyb.h"

#include "ps2_converter.h"

#include "main.h"


int main(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	printf("INIT!\n");

	amikbd_setup(&PORTC, &DDRC, 0);

//	ps2keyb_init(&PORTD, &DDRD, &PIND, 1);
	ps2keyb_init(&PORTC, &DDRC, &PINC, 1);
	ps2keyb_setCallback(ps2k_callback);

	sei();

	amikbd_init();

	printf("Initialization completed...\n");

	 while(1);

    return 0;
}


