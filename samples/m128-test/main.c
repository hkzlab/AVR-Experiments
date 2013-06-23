#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "main.h"


int main(void) {
	DDRA = 0xFF;
	PORTA = 0xFE;

	while(1) {
		PORTA = ~PORTA;	
		_delay_ms(1000);
	}

	return 0;
}


