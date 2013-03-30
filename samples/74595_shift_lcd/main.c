#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "main.h"
#include "uart.h"

#include "shifter_74595.h"
#include "hd44780-74595-interface.h"
#include "hd44780-highlevel.h"

int main(void) {
	// Initialize serial port for output
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	DDRC = 0xFF;
	PORTC = 0x00;

	shifter_74595_conn *shfConn = shf74595_createConnection(&PORTC, 3, &PORTC, 2, &PORTC, 1, &PORTC, 0);
	shf74595_initConnection(shfConn);

	hd44780_74595_connection *conn_struct = hd44780_74595_createConnection(shfConn, 0, 5, 4);
	hd44780_driver *connDriver = hd44780_hl_createDriver(TMBC20464BSP_20x4, conn_struct, (uint8_t (*)(void*))hd44780_74595_initLCD4Bit, (void (*)(void*, uint16_t))hd44780_74595_sendCommand);

	hd44780_hl_init(connDriver, 0, 0);
	

	hd44780_hl_printText(connDriver, 0, 0, "Ciao!\nIo funziono con uno\nshifter di tipo\n74HC595!");

	_delay_ms(3000);

	uint8_t data[2][8] = {{0b00001110,
						   0b00011111,
						   0b00010101,
						   0b00011111,
						   0b00001010,
						   0b00001110,
						   0b00001010,
						   0b0000000},
						  {0b00001010,
						   0b00011111,
						   0b00011111,
						   0b00011111,
						   0b00001110,
						   0b00001110,
						   0b00000100,
						   0b00000000}};

	hd44780_hl_setCustomFont(connDriver, 0, data[0]);
	hd44780_hl_setCustomFont(connDriver, 1, data[1]);

	uint8_t counter = 0;
	while (1) {
		hd44780_hl_printChar(connDriver, 3, 8, counter);

		counter = (counter + 1) % 2;
		_delay_ms(1000);
	}

    return 0;
}
