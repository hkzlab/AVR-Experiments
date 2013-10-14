#include "amiga_keyb.h"

#include <stdio.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define AMI_KBDCODE_SELFTESTFAILED 0xFC
#define AMI_KBDCODE_INITKEYSTREAM  0xFD
#define AMI_KBDCODE_ENDKEYSTREAM   0xFE
#define AMI_KBDCODE_LOSTSYNC       0xF9

// Data port
static volatile uint8_t *dPort, *dDir;
static volatile uint8_t dPNum; // Data port pin number

// Clock port
static volatile uint8_t *cPort, *cDir;
static volatile uint8_t cPNum; // Clock port pin number

static volatile uint8_t amikbd_synced = 0;

static inline void amikbd_kClock(void);
static inline void amikbd_kToggleData(uint8_t bit);
uint8_t amikbd_kSync(void);

void amikbd_setup(volatile uint8_t *clockPort, volatile uint8_t *clockDir, uint8_t clockPNum) {
#if defined (__AVR_ATmega128__)
	dPort = &PORTD;
	dDir = &DDRD;
	dPNum = 1; // PD1, for INT1
#elif defined (__AVR_ATtiny4313__) || defined (__AVR_ATmega328P__)
	dPort = &PORTD;
	dDir = &DDRD;
	dPNum = 3; // PD3, for INT1
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

#if defined (__AVR_ATmega128__) || defined (__AVR_ATmega328P__)
	EICRA &= ~((1 << ISC10) | (1 << ISC11)); // Trigger interrupt at LOW LEVEL (INT1)
	EIMSK &= ~(1 << INT1); // Disable INT1
	EIFR |= (1 << INTF1); // Clear interrupt flag
#elif defined (__AVR_ATtiny4313__)
	MCUCR &= ~((1 << ISC10) | (1 << ISC11)); // Trigger interrupt at LOW LEVEL (INT1)
	GIMSK &= ~(1 << INT1); // Disable INT1
	GIFR |= (1 << INTF1); // Clear interrupt flag
#else
	// ???
#endif
}

// http://amigadev.elowar.com/read/ADCD_2.1/Hardware_Manual_guide/node0177.html#line46
void amikbd_init(void) {
	amikbd_kSync();

	// We should send the test failed code here, if any problem is detected

	// Send initializate powerup key stream
	amikbd_kSendCommand(AMI_KBDCODE_INITKEYSTREAM);
	// Here we should send the keycodes for all the "pressed" keyboard keys...
	// But I will simply suppose there is none

	amikbd_kSendCommand(AMI_KBDCODE_ENDKEYSTREAM);
	//amikbd_kSendCommand(0x35); // FIXME: Useless?

}

ISR(INT1_vect) { // Manage INT1
	amikbd_synced = 1;

#if defined (__AVR_ATmega128__) || defined (__AVR_ATmega328P__)
	EIMSK &= ~(1 << INT1); // Disable INT1
#elif defined (__AVR_ATtiny4313__)
	GIMSK &= ~(1 << INT1); // Disable INT1
#endif
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
	_delay_ms(600);
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
	uint8_t retries = 0xFF;

#if defined (__AVR_ATmega128__) || defined (__AVR_ATmega328P__)
	EIMSK &= ~(1 << INT1); // Disable INT1
#elif defined (__AVR_ATtiny4313__)
	GIMSK &= ~(1 << INT1); // Disable INT1
#endif
	
	*dPort &= ~(1 << dPNum); // Set output to low
	*dDir &= ~(1 << dPNum); // KB Data line set as input
	_delay_us(20);

#if defined (__AVR_ATmega128__) || defined (__AVR_ATmega328P__)
	EIMSK |= (1 << INT1); // Enable INT1
#elif defined (__AVR_ATtiny4313__)
	GIMSK |= (1 << INT1); // Enable INT1
#endif
	
	_delay_us(170);

	if (amikbd_synced) return 1; // The keyboard got synced

	while (retries--) {
#if defined (__AVR_ATmega128__) || defined (__AVR_ATmega328P__)
		EIMSK &= ~(1 << INT1); // Disable INT1
#elif defined (__AVR_ATtiny4313__)
		GIMSK &= ~(1 << INT1); // Disable INT1
#endif

		*dPort &= ~(1 << dPNum); // Set output to low
		*dDir |= (1 << dPNum); // KB Data line set as output
		
		amikbd_kClock(); // Send a clock signal

		*dDir &= ~(1 << dPNum); // KB Data line set as input
		*dPort |= (1 << dPNum); // FIXME: This sets the pull up resistr... WTF?

		_delay_us(20);

#if defined (__AVR_ATmega128__) || defined (__AVR_ATmega328P__)
		EIFR |= (1 << INTF1); // Clear interrupt flag
#elif defined (__AVR_ATtiny4313__)
		GIFR |= (1 << INTF1); // Clear interrupt flag
#endif
		amikbd_synced = 0;

#if defined (__AVR_ATmega128__) || defined (__AVR_ATmega328P__)
		EIMSK |= (1 << INT1); // Enable INT1
#elif defined (__AVR_ATtiny4313__)
		GIMSK |= (1 << INT1); // Enable INT1
#endif
		_delay_us(120);

		if (amikbd_synced) return 1;
	}

	return 0; // Sync failed
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

