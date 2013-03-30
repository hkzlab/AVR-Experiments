#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#include "main.h"
#include "uart.h"

#include "shifter_74595.h"
#include "hd44780-74595-interface.h"
#include "hd44780-highlevel.h"

#include "twilib.h"
#include "ds1307.h"

#define  SRAM_CHKVAL 0xDEAD

void sys_setup(void);

static hd44780_driver *lcdDriver = NULL;

void sys_setup(void) {
	// Initialize serial port for output
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	// Shifter connections
	DDRC |= 0x0F; // First 4 pins of Port C set as output
	PORTC &= 0xF0;

	// Key encoder connections
	DDRB &= 0xF0; // First 4 pins of Port B set as Input
	PORTB &= 0xF0;
	DDRD &= 0x7F; // Last pin of Port D set as Input (will be used as interrupt)
	PORTD &= 0x7F;

	// Enable interrupts
	PCICR |= 1 << PCIE2; // Enable PCINT2 (23..16)
	PCMSK2 |= 1 << PCINT23;
	sei();

	// Init LCD
	shifter_74595_conn *shfConn = shf74595_createConnection(&PORTC, 3, &PORTC, 2, &PORTC, 1, &PORTC, 0);
	shf74595_initConnection(shfConn);

	hd44780_74595_connection *conn_struct = hd44780_74595_createConnection(shfConn, 0, 5, 4);
	lcdDriver = hd44780_hl_createDriver(TMBC20464BSP_20x4, conn_struct, (uint8_t (*)(void*))hd44780_74595_initLCD4Bit, (void (*)(void*, uint16_t))hd44780_74595_sendCommand);
	hd44780_hl_init(lcdDriver, 0, 0);	

	// Init TWI
	I2C_Config *masterConfig = I2C_buildDefaultConfig();
	I2C_masterBegin(masterConfig);

	// Print first line
	hd44780_hl_clear(lcdDriver);
	hd44780_hl_printText(lcdDriver, 0, 0, "Base Sensori\nAttendere...");
	_delay_ms(1000);	

	// Check the clock
	DDRD &= 0xBF; // Set pin 6 of Port D as input. Will be used as interrupt
	PORTD &= 0xBF;

	fprintf(stdout, "Checking DS1307 status...\n");
	uint16_t ds_chkval;
	DS1307_readSRAM((uint8_t*)&ds_chkval, 2);
	if (ds_chkval != SRAM_CHKVAL) { // The clock lost power!
		ds_chkval = SRAM_CHKVAL;

		DS1307_writeSRAM((uint8_t*)&ds_chkval, 2); // Mark clock SRAM
	} else { // Clock is fine
		fprintf(stdout, "Clock check passed!!!\n");
	}
	DS1307_setSQW(1, 0, DS1307_SQW_1Hz);
	PCMSK2 |= 1 << PCINT22;	
	

	//DS1307_ToD time;
	//DS1307_readToD(&time);

	hd44780_hl_clear(lcdDriver);
}

volatile uint8_t guappo = 0;

int main(void) {
	sys_setup();


	while (1) {
		_delay_ms(1000);
	}

    return 0;
}

ISR(PCINT2_vect) { // PCINT 23..16
}
