#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include "spi.h"

#include "uart.h"
#include "main.h"

static volatile uint8_t int0_received = 0;
static volatile uint8_t intw_received = 0;

static void interrupt_init(void);
static void setup_wdt(void);
static void disable_ports(void);
static void enter_sleep(void);

int main(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	// Setting up interrupts
	interrupt_init();

	// Disable unused ports to lower consumption
	disable_ports();

	// Setup SPI
	setup_spi(SPI_MODE_0, SPI_MSB, SPI_NO_INTERRUPT, SPI_MSTR_CLK2);

	fprintf(stdout,"START\n");

	setup_wdt(); // Setup watchdog functions

	sei(); // Enable interrupts

	while (1) {
		enter_sleep();	

	}

//	wdt_disable();

    return 0;
}

static void disable_ports(void) {
	DDRD &= 0x03; // Port D as input (except RX/TX)
	DDRB = 0x00; // Port B as input
	DDRC = 0x00; // Port C as input

	PORTD |= 0xFC; // Enable pullups on port D
	PORTB |= 0xFF; // Enable pullups on port D
	PORTC |= 0xFF; // Enable pullups on port C
}

static void setup_wdt(void) {
	/* Clear the reset flag. */
	MCUSR &= ~(1<<WDRF);
  
	/* In order to change WDE or the prescaler, we need to
	 * set WDCE (This will allow updates for 4 clock cycles).
	 */
	WDTCSR |= (1<<WDCE) | (1<<WDE);

	/* set new watchdog timeout prescaler value */
	WDTCSR = (1<<WDP0) | (1<<WDP3); /* 8.0 seconds */
  
	/* Enable the WD interrupt (note no reset). */
	WDTCSR |= (1<<WDIE);
}

static void interrupt_init(void) {
	/* Initialize external interrupt on pin INT0 on failing edge */
	EICRA |= (1 << ISC01);
	EIMSK |= (1 << INT0);
}

static void enter_sleep(void) {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* Lowest power consumption */
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
}


/* System interrupt handler */
SIGNAL(INT0_vect) {
	int0_received = 1;
} 

/* WATCHDOG interrupt handler */
SIGNAL(WDT_vect) {
	intw_received = 1;
} 


