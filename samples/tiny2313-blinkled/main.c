#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "USI_TWI_Master.h"

#include "main.h"

int main(void) {
	USI_TWI_Master_Initialise();

	DDRD = 0x40;

	while (1) {
		PORTD &= 0xBF;
		_delay_ms(1000);
		PORTD |= 0x40;
		_delay_ms(1000);
	}

    return 0;
}
