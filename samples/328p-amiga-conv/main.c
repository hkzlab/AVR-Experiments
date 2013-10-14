#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "ps2_keyb.h"
#include "ps2_proto.h"

#include "amiga_keyb.h"

#include "ps2_converter.h"

#include "main.h"


int main(void) {
	// Set the pull-up resistor to all unused I/O ...
	DDRB &= 0x03;
	PORTB |= 0xFC;
	
	DDRC &= 0xC0;
	PORTC |= 0x3F;

	DDRD &= 0x0C;
	PORTD |= 0xF3;

	_delay_us(100);

	// Initialization of PS/2 and Amiga interface
	amikbd_setup(&PORTB, &DDRB, 0);

	ps2keyb_init(&PORTB, &DDRB, &PINB, 1);
	ps2keyb_setCallback(ps2k_callback);

	sei();

	// Disable typematic break
	uint8_t ps2_command = PS2_HTD_ALLKEYSMAKEBREAK;
	ps2keyb_sendCommand(&ps2_command, 1);

	amikbd_init();

	while(1);

    return 0;
}


