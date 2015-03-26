#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "spi.h"
#include "mcp2515.h"


#include "uart.h"
#include "main.h"

static volatile uint8_t interrupt_received = 0;
void (*int_handler)(void); 

static void extInterruptINIT(void (*handler)(void));
static void interrupt_handler(void);

int main(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	extInterruptINIT(interrupt_handler);
	sei(); // Enable interrupts
	
	// Setup SPI
	setup_spi(SPI_MODE_0, SPI_MSB, SPI_NO_INTERRUPT, SPI_MSTR_CLK4);
	PORT_SPI &= ~(1<<SPI_SS_PIN);


	while (1) {
		_delay_ms(1000);		
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




