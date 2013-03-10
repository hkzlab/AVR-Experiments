 
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

#include "analog_sensors.h"

volatile uint8_t interruptReceived = 0;

#define SERIES_RESISTOR 10012.0f // ~10 kOHM
#define REF_VOLTAGE 5.0f
#define THERMISTOR_NOMINAL 12000.0f
#define TEMPERATURE_NOMINAL 25.0f
#define B_COEFFICIENT 3760


int main(void) {
	char lcdString[80];

	// Initialize serial port for output
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	setupADC(0x02);

	// Setup the LCD
	DDRB = 0xFF; // Set port B as output!
	PORTB = 0x00; // And zero it
	hd44780_connection *conn_struct = hd44780_createConnection(&PORTB, 0, &PORTB, 5, &PORTB, 4, &PORTB, 6);
	hd44780_driver *connDriver = hd44780_hl_createDriver(TMBC20464BSP_20x4, conn_struct, (uint8_t (*)(void*))hd44780_initLCD4Bit, (void (*)(void*, uint16_t))hd44780_sendCommand);
	hd44780_hl_init(connDriver, 0, 0);

	I2C_Config *masterConfig = I2C_buildDefaultConfig();
	I2C_masterBegin(masterConfig);

	DS1307_ToD time;	
	DS1307_setSQW(1, 0, DS1307_SQW_1Hz);
	DS1307_readToD(&time);

	DDRC &= 0xF7;
	PORTC |= 0x8;
	// See here for interrupts http://www.protostack.com/blog/2010/09/external-interrupts-on-an-atmega168/
	PCICR |= 1 << PCIE1;
	PCMSK1 |= 1 << PCINT11;
	sei();


	_delay_ms(500);
	fprintf(stdout, "Start loop...\n");	

	uint16_t adc_val = readADC();
	while (1) {
		if (interruptReceived) {
			interruptReceived = 0;
			DS1307_readToD(&time);

			sprintf(lcdString, "%.2u-%.2u-%.4u\n%.2u:%.2u:%.2u\n\n%.1fC", time.dayOfMonth, time.month, time.year, time.hours, time.minutes, time.seconds, adcReadToTemp(adc_val, REF_VOLTAGE, SERIES_RESISTOR, THERMISTOR_NOMINAL, TEMPERATURE_NOMINAL, B_COEFFICIENT));
			hd44780_hl_printText(connDriver, 0, 0, lcdString);
		}

		adc_val = (adc_val +  readADC()) / 2;
	}

    return 0;
}

ISR(PCINT1_vect) {
	static uint8_t counter = 0;

	if (!(++counter % 2))
		interruptReceived = 1;
}
