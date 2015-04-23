#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "uart.h"
#include "main.h"

static volatile uint8_t interrupt_received = 0, first_pkt = 1;
void (*int_handler)(void); 

// BIT MAPPING:
// 0 -> A
// 1 -> F
// 2 -> E
// 3 -> D
// 4 -> C
// 5 -> G
// 6 -> B
static uint8_t segm_mapper[] = {0x5F, 0x50, 0x6D, 0x79, 0x72, 0x3B, 0x3F, 0x51, 0x7F, 0x73, 0x77, 0x3E, 0x0F, 0x7C, 0x2F, 0x27};

static void extInterruptINIT(void (*handler)(void));
static void interrupt_handler(void);

int main(void) {
	uint8_t idx, digit_idx;
	uint32_t cnt = 0;
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	DDRF = 0xFF; // Port F as output
	PORTF = 0xFF; // All pins low on port F
	DDRK = 0xFF; // Port K as output
	PORTK = 0x00; // All pins low on port K

	extInterruptINIT(interrupt_handler);
//	sei(); // Enable interrupts
	
//	wdt_enable(WDTO_500MS);

	fprintf(stdout, "RUN\n");

	PORTF = segm_mapper[15];

	cnt = 0; digit_idx = 0;
	while (1) {
	//	cli();
	for(idx = 1; idx < 0x08; idx <<= 1) {
		PORTK = ~idx;
//		_delay_ms(500);
	}

		cnt++;
		if(cnt == 0x3FFFF) {
			cnt = 0;
			digit_idx = (digit_idx+1)%16;
			PORTF = segm_mapper[digit_idx];
		}

	}

    return 0;
}

static void extInterruptINIT(void (*handler)(void)) {
	/* Set function pointer */
	int_handler = handler;
	/* Initialize external interrupt on pin INT0 on failing edge */
	EICRA |= (1 << ISC01);
	EIMSK |= (1 << INT0);
}

static void interrupt_handler(void) {
	interrupt_received = 1;
}

/* System interrupt handler */
SIGNAL(INT0_vect) {
	int_handler();
} 




