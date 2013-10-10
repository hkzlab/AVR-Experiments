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
	amikbd_setup(&PORTB, &DDRB, 0);

	ps2keyb_init(&PORTB, &DDRB, &PINB, 1);
	ps2keyb_setCallback(ps2k_callback);

	sei();

	amikbd_init();

	while(1);

    return 0;
}


