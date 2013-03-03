 
#ifndef F_CPU
#define F_CPU 8000000UL // MCU Frequency set to 8Mhz
#endif

#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <util/delay.h>

#include "main.h"
#include "uart.h"

#include "twilib.h"
#include "ds1307.h"

#include "hd44780-avr-interface.h"
#include "hd44780-highlevel.h"

volatile uint8_t interruptReceived = 0;

int main(void) {
	char lcdString[80];

	// Initialize serial port for output
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	// Setup ADC
	// See here: http://www.avrbeginners.net/
	// http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=56429
	// http://www.protostack.com/blog/2011/02/analogue-to-digital-conversion-on-an-atmega168/
//	ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);
//	ADMUX = (1 << REFS0) | 0x7;


	// Setup the LCD
	DDRB = 0xFF; // Set port B as output!
	PORTB = 0x00; // And zero it
	hd44780_connection *conn_struct = hd44780_createConnection(&PORTB, 1, &PORTB, 0, &PORTB, 1, &PORTB, 2);
	hd44780_driver *connDriver = hd44780_hl_createDriver(TMBC20464BSP_20x4, conn_struct, (uint8_t (*)(void*))hd44780_initLCD4Bit, (void (*)(void*, uint16_t))hd44780_sendCommand);
	hd44780_hl_init(connDriver, 0, 0);
	
	I2C_Config *masterConfig = I2C_buildDefaultConfig();
	I2C_masterBegin(masterConfig);

	DS1307_ToD time;	
	DS1307_setSQW(1, 0, DS1307_SQW_1Hz);
	DS1307_readToD(&time);

	DDRC &= 0x7F;
	PORTC |= 0x80;
	// See here for interrupts http://www.protostack.com/blog/2010/09/external-interrupts-on-an-atmega168/
	PCICR |= 1 << PCIE2;
	PCMSK2 |= 1 << PCINT23;
	sei();

	_delay_ms(500);
	fprintf(stdout, "Start loop...\n");	

	while (1) {
		if (interruptReceived) {
			interruptReceived = 0;
			DS1307_readToD(&time);
			sprintf(lcdString, "%.2u-%.2u-%.4u\n%.2u:%.2u:%.2u", time.dayOfMonth, time.month, time.year, time.hours, time.minutes, time.seconds);
			hd44780_hl_printText(connDriver, 0, 0, lcdString);


		}
/*			ADCSRA |= (1 << ADSC);
			while (ADCSRA & (1 << ADSC)); // Busy wait...
			uint16_t analogRead = (ADCH << 8 | ADCL);
			fprintf(stdout, "ANALOG %.4X\n", analogRead);
*/
	}

    return 0;
}

ISR(PCINT2_vect) {
	interruptReceived = 1;
}
