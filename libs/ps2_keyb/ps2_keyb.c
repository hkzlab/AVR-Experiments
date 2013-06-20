#include "ps2_keyb.h"

#include <avr/io.h>

// Data port
static volatile uint8_t *dPort, *dPin, *dDir;
static uint8_t dPNum; // Data port pin (leg) number

// Clock port
static volatile uint8_t *cPort, *cPin, *cDir;
static uint8_t cPNum;

// See http://avrprogrammers.com/example_avr_keyboard.php
// http://elecrom.wordpress.com/2008/02/12/avr-tutorial-2-avr-input-output/
void ps2keyb_init(volatile uint8_t *dataPort, volatile uint8_t *dataDir, volatile uint8_t *dataPin, uint8_t pNum) {
	dPort = dataPort;
	dPin = dataPin;
	dDir = dataDir;
	dPNum = pNum;

	cPort = &PORTD;
	cPin = &PIND;
	cDir = &DDRD;
	cPNum = 2; // PD2

	// Prepare data port
	*dDir &= ~(1 << dPNum); // KB Data line set as input
	*dPort |= (1 << dPNum); // Pull-up resistor on data line

	// Prepare clock port
	*cDir &= ~(1 << cPNum); // KB Clock line set as input
	*cPort |= (1 << cPNum); // Pull-up resistor on clock line

	// See http://www.avr-tutorials.com/interrupts/The-AVR-8-Bits-Microcontrollers-External-Interrupts
	// And http://www.atmel.com/images/doc2543.pdf
	MCUCR &= ~((1 << ISC00) | (1 << ISC01)); 
	MCUCR |= (1 << ISC01);  // Trigger interrupt at FALLING EDGE (INT0)

	// I suspect this to be totally useless...
	//PCMSK |= (1<<PIND2);	// Enable pin change on INT0 (why is this required?)	
	GIMSK |= (1 << INT0); // Enable INT0

}

