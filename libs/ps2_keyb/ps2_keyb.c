#include "ps2_keyb.h"

#include <avr/io.h>
#include <avr/interrupt.h>

// Data port
static volatile uint8_t *dPort, *dPin, *dDir;
static volatile uint8_t dPNum; // Data port pin (leg) number

// Clock port
static volatile uint8_t *cPort, *cPin, *cDir;
static volatile uint8_t cPNum;

#define KB_CLOCK_FALL 0
#define KB_CLOCK_RISE 1
#define PS2_START_BITCOUNT 11 // 12 bits is only for host-to-device communication

#define KEY_BUF_SIZE 8

static volatile uint8_t clock_edge;
static volatile uint8_t kb_bitCount;

static volatile uint8_t keyBuffer[KEY_BUF_SIZE];
static volatile uint8_t *kb_inPtr, *kb_outPtr, *kb_endPtr;

#define KB_START_BIT(a) ((a >> 0) & 0x01)
#define KB_PARITY_BIT(a) ((b >> 1) & 0x01)
#define KB_STOP_BIT(a) ((c >> 2) & 0x01)

#define KB_SET_START_BIT(a, b) (a |= (b << 0))
#define KB_SET_PARITY_BIT(a, b) (a |= (b << 1))
#define KB_SET_STOP_BIT(a, b) (a |= (b << 2))


static volatile uint8_t kb_data;
static volatile uint8_t kb_flag;

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

	clock_edge = KB_CLOCK_FALL;
	kb_bitCount = PS2_START_BITCOUNT;

	kb_data = 0;
	kb_flag = 0;

	// Prepare the ring buffer...
	kb_inPtr = kb_outPtr = keyBuffer;
	kb_endPtr = kb_inPtr + KEY_BUF_SIZE;

	GIMSK |= (1 << INT0); // Enable INT0

}

// See http://www.computer-engineering.org/ps2protocol/
ISR(INT0_vect) { // Manage INT0
	uint8_t kBit = 0;

	if (clock_edge == KB_CLOCK_FALL) { // Falling edge
		kBit = (*dPin & (1 << dPNum)) ? 1 : 0;

		// bit 0 is start bit, bit 9,10 are parity and stop bits
		// What is left are the data bits!
		if (kb_bitCount < 11 && kb_bitCount > 2) { 
			kb_data >>= 1; // Shift the data

			if (kBit) kb_data |= 0x80; // Add a bit if the read data is one
		} else if (kb_bitCount == 11) { // start bit, must always be 0!
			KB_SET_START_BIT(kb_flag, kBit);
		} else if (kb_bitCount == 9) { // Parity bit: 1 if there is an even number of 1s in the data bits
			KB_SET_PARITY_BIT(kb_flag, kBit);			
		} else if (kb_bitCount == 10) { // Stop bit, must always be 1!
			KB_SET_STOP_BIT(kb_flag, kBit);			
		}
		clock_edge = KB_CLOCK_RISE;			// Ready for rising edge.

		MCUCR |= ((1 << ISC00) | (1 << ISC01)); // Setup INT0 for rising edge.
	} else { // Rising edge
		if( (--kb_bitCount)) {
			// TODO: Do something with the data

			kb_data = 0;
			kb_flag = 0;

			kb_bitCount = PS2_START_BITCOUNT; // Start over.
		}
		clock_edge = KB_CLOCK_FALL;		// Setup routine the next falling edge.

		MCUCR &= ~((1 << ISC00) | (1 << ISC01)); 
		MCUCR |= (1 << ISC01);  // Trigger interrupt at FALLING EDGE (INT0)
	}
}

