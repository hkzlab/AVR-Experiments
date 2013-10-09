#include "amiga_keyb.h"

#include <stdio.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Data port
static volatile uint8_t *dPort, *dDir, *dPin;
static volatile uint8_t dPNum; // Data port pin (leg) number

// Clock port
static volatile uint8_t *cPort, *cDir;
static volatile uint8_t cPNum;

static volatile uint8_t amikbd_synced = 0;

static inline void amikbd_kClock(void);
static inline void amikbd_kToggleData(uint8_t bit);
uint8_t amikbd_kSync(void);

void amikbd_setup(volatile uint8_t *clockPort, volatile uint8_t *clockDir, uint8_t clockPNum) {
	dPort = &PORTD;
	dPin = &PIND;
	dDir = &DDRD;
#if defined (__AVR_ATmega128__)
	dPNum = 1; // PD1, for INT1
#else
	// ???
#endif

	cPort = clockPort;
	cDir = clockDir;
	cPNum = clockPNum;

	// Prepare data port
	*dDir &= ~(1 << dPNum); // KB Data line set as input
	*dPort &= ~(1 << dPNum); // Disable pull-up resistor in data line

	// Prepare clock port
	*cDir |= (1 << cPNum); // KB Clock line set as output: we are performing the timing
	*cPort |= (1 << cPNum); // Pull the clock line high!

#if defined (__AVR_ATmega128__)
	EICRA &= ~((1 << ISC10) | (1 << ISC11)); // Trigger interrupt at LOW LEVEL (INT1)
	EIMSK &= ~(1 << INT1); // Disable INT1
	EIFR |= (1 << INT1); // Clear interrupt flag
#else
	// ???
#endif
}

void amikbd_init(void) {
	amikbd_kSync();

	// Send initialization command
	amikbd_kSendCommand(0xFD);
	amikbd_kSendCommand(0xFE);
	amikbd_kSendCommand(0x35);

}

ISR(INT1_vect) { // Manage INT1
	amikbd_synced = 1;

	EIMSK &= ~(1 << INT1); // Disable INT1
}

// Clock the keyboard line
static inline void amikbd_kClock(void) {
	_delay_us(20);
	*cPort &= ~(1 << cPNum); // Pull the clock line low!
	
	_delay_us(20);
	*cPort |= (1 << cPNum); // Pull the clock line high!
	
	_delay_us(20);
}

void amikbd_kForceReset(void) {
	*cPort &= ~(1 << cPNum); // Pull the clock line low!
	_delay_us(500);
	*cPort |= (1 << cPNum); // Pull the clock line high!	
}

static inline void amikbd_kToggleData(uint8_t bit) {
	if (bit)
		*dPort &= ~(1 << dPNum);
	else
		*dPort |= (1 << dPNum);

	amikbd_kClock();
}

uint8_t amikbd_kSync(void) {
	EIMSK &= ~(1 << INT1); // Disable INT1
	*dPort &= ~(1 << dPNum); // Set output to low
	*dDir &= ~(1 << dPNum); // KB Data line set as input
	_delay_us(20);

	EIMSK |= (1 << INT1); // Enable INT1 again
	_delay_us(170);

	if (amikbd_synced) return 1; // The keyboard got synced

	while (1) {
		EIMSK &= ~(1 << INT1); // Disable INT1
		*dPort &= ~(1 << dPNum); // Set output to low
		*dDir |= (1 << dPNum); // KB Data line set as output
		
		amikbd_kClock(); // Send a clock signal

		*dDir &= ~(1 << dPNum); // KB Data line set as input
		*dPort |= (1 << dPNum); // FIXME: This sets the pull up resistr... WTF?

		_delay_us(20);

		EIFR |= (1 << INT1); // Clear interrupt flag
		amikbd_synced = 0;
		EIMSK |= (1 << INT1); // Enable INT1 again

		_delay_us(120);

		if (amikbd_synced) return 0;
	}
}


void amikbd_kSendCommand(uint8_t command) {
	if (command == 0xFF) return;

	*dPort |= (1 << dPNum); // FIXME: Set output as high
	*dDir |= (1 << dPNum); // KB Data line set as output

	amikbd_kToggleData((command >> 6) & 1);
	amikbd_kToggleData((command >> 5) & 1);
	amikbd_kToggleData((command >> 4) & 1);
	amikbd_kToggleData((command >> 3) & 1);
	amikbd_kToggleData((command >> 2) & 1);
	amikbd_kToggleData((command >> 1) & 1);
	amikbd_kToggleData((command >> 0) & 1);
	amikbd_kToggleData((command >> 7) & 1);

	amikbd_kSync();
}