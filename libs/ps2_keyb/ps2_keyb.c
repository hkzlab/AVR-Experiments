#include "ps2_keyb.h"
#include "ps2_proto.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>

// See the following link for details on PS/2 protocol
// http://www.computer-engineering.org/ps2protocol/
// See the following link for details on the keyboard PS/2 commands
// http://www.computer-engineering.org/ps2keyboard/

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
#define KB_PARITY_BIT(a) ((a >> 1) & 0x01)
#define KB_STOP_BIT(a) ((a >> 2) & 0x01)

#define KB_SET_START_BIT(a, b) (a |= (b << 0))
#define KB_SET_PARITY_BIT(a, b) (a |= (b << 1))
#define KB_SET_STOP_BIT(a, b) (a |= (b << 2))

void ps2_dumb_print(uint8_t *code, uint8_t count);

void static (*keypress_callback)(uint8_t *code, uint8_t count) = ps2_dumb_print;
static volatile uint8_t kb_data, kb_flag;

int kb_parity_check(uint8_t kb_flag_i, uint8_t kb_data_i);
void kb_pushScancode(uint8_t code);

void ps2_dumb_print(uint8_t *code, uint8_t count) {
	printf("%.2X %.2X %.2X\n", code[0], code[1], code[2]);
}

int kb_parity_check(uint8_t kb_flag_i, uint8_t kb_data_i) {
	uint8_t result = 1;
	uint8_t counter = 8;

	while (counter--) {
		result = kb_data_i & 0x1 ? !result : result;
		kb_data_i >>= 1;
	}

	return (result == KB_PARITY_BIT(kb_flag_i));
}

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
#if defined (__AVR_ATmega128__)
	cPNum = 0; // PD0
#elif defined (__AVR_ATtiny4313__) || defined (__AVR_ATmega328P__)
	cPNum = 2; // PD2
#endif

	// Prepare data port
	*dDir &= ~(1 << dPNum); // KB Data line set as input
	*dPort |= (1 << dPNum); // Pull-up resistor on data line

	// Prepare clock port
	*cDir &= ~(1 << cPNum); // KB Clock line set as input
	*cPort |= (1 << cPNum); // Pull-up resistor on clock line

	// See http://www.avr-tutorials.com/interrupts/The-AVR-8-Bits-Microcontrollers-External-Interrupts
	// And http://www.atmel.com/images/doc2543.pdf

#if defined (__AVR_ATmega128__) || defined (__AVR_ATmega328P__)
	EICRA &= ~((1 << ISC00) | (1 << ISC01)); 
	EICRA |= (1 << ISC01);  // Trigger interrupt at FALLING EDGE (INT0)
	EIMSK |= (1 << INT0);
#elif defined (__AVR_ATtiny4313__)
	MCUCR &= ~((1 << ISC00) | (1 << ISC01)); 
	MCUCR |= (1 << ISC01);  // Trigger interrupt at FALLING EDGE (INT0)
#endif

	// I suspect this to be totally useless...
	//PCMSK |= (1<<PIND2);	// Enable pin change on INT0 (why is this required?)

	clock_edge = KB_CLOCK_FALL;
	kb_bitCount = PS2_START_BITCOUNT;

	kb_data = 0;
	kb_flag = 0;

	// Prepare the ring buffer...
	kb_inPtr = kb_outPtr = keyBuffer;
	kb_endPtr = kb_inPtr + KEY_BUF_SIZE;

	// Enable INT0
#if defined (__AVR_ATmega128__) || defined (__AVR_ATmega328P__)
	EIMSK |= (1 << INT0);
#elif defined (__AVR_ATtiny4313__)
	GIMSK |= (1 << INT0);
#endif
}

void kb_pushScancode(uint8_t code) {
	static uint8_t code_array[9];
	static uint8_t cur = 0;

	code_array[cur] = code;

	switch (code) {
		case PS2_SCANCODE_RELEASE: // Key released, expect at least another code!
		case PS2_SCANCODE_EXTENDED: // Extended scancode, one or two more!
		case PS2_SCANCODE_PAUSE: // Pause ...
			cur++;
			break;
		default:
			// Manage pause and printscreen
			if (((code_array[0] == PS2_SCANCODE_EXTENDED) && (code_array[1] == 0x12) && (code_array[3] != 0x7C)) ||
				((code_array[0] == PS2_SCANCODE_EXTENDED) && (code_array[1] == 0xF0) && (code_array[2] == 0x7C) && (code_array[5] != 0x12)) ||
				(code_array[0] == PS2_SCANCODE_PAUSE && cur < 7)) {
				cur++;
				break;
			}

			(*keypress_callback)(code_array, cur);

			cur = 0;
			code_array[0] = code_array[1] = code_array[2] = 0;
			code_array[3] = code_array[4] = code_array[5] = 0;
			code_array[6] = code_array[7] = code_array[8] = 0;

			break;
	}
}

void ps2keyb_setCallback(void (*callback)(uint8_t *code, uint8_t count)) {
	keypress_callback = callback;
}

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
		} else if (kb_bitCount == 2) { // Parity bit: 1 if there is an even number of 1s in the data bits
			KB_SET_PARITY_BIT(kb_flag, kBit);			
		} else if (kb_bitCount == 1) { // Stop bit, must always be 1!
			KB_SET_STOP_BIT(kb_flag, kBit);	
		}
		clock_edge = KB_CLOCK_RISE;			// Ready for rising edge.

#if defined (__AVR_ATmega128__) || defined (__AVR_ATmega328P__)
		EICRA |= ((1 << ISC00) | (1 << ISC01)); // Setup INT0 for rising edge.
#elif defined (__AVR_ATtiny4313__)
		MCUCR |= ((1 << ISC00) | (1 << ISC01)); // Setup INT0 for rising edge.
#endif
	} else { // Rising edge
		if(!(--kb_bitCount)) {
			if (!KB_START_BIT(kb_flag) && KB_STOP_BIT(kb_flag) && kb_parity_check(kb_flag, kb_data)) {
				kb_pushScancode(kb_data);
			} // Else... there was a problem somewhere, probably timing

			kb_data = 0;
			kb_flag = 0;

			kb_bitCount = PS2_START_BITCOUNT; // Start over.
		}
		clock_edge = KB_CLOCK_FALL;		// Setup routine the next falling edge.

#if defined (__AVR_ATmega128__) || defined (__AVR_ATmega328P__)
		EICRA &= ~((1 << ISC00) | (1 << ISC01)); 
		EICRA |= (1 << ISC01);  // Trigger interrupt at FALLING EDGE (INT0)
#elif defined (__AVR_ATtiny4313__)
		MCUCR &= ~((1 << ISC00) | (1 << ISC01)); 
		MCUCR |= (1 << ISC01);  // Trigger interrupt at FALLING EDGE (INT0)
#endif
	}
}



