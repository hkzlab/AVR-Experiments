 
#ifndef F_CPU
#define F_CPU 8000000UL // MCU Frequency set to 8Mhz
#endif

#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "hd44780-avr-interface.h"
#include "hd44780-highlevel.h"

#include "main.h"
#include "uart.h"

int main(void) {
	hd44780_connection *conn_struct = hd44780_createConnection(&PORTB, 1, &PORTB, 0, &PORTB, 1, &PORTB, 2);
	hd44780_driver *connDriver = hd44780_hl_createDriver(TMBC20464BSP_20x4, conn_struct, (uint8_t (*)(void*))hd44780_initLCD4Bit, (void (*)(void*, uint16_t))hd44780_sendCommand);

	DDRB = 0xFF; // Set port B as output!
	PORTB = 0x00; // And zero it

	// Initialize serial port for output
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	hd44780_hl_init(connDriver, 0, 0);

	uint8_t data[6][8] = {{0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F},
						  {0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E},
						  {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C},
						  {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},
						  {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10},
						  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
		

	hd44780_hl_setCustomFont(connDriver, 0, data[5]);
	hd44780_hl_setCustomFont(connDriver, 1, data[4]);
	hd44780_hl_setCustomFont(connDriver, 2, data[3]);
	hd44780_hl_setCustomFont(connDriver, 3, data[2]);
	hd44780_hl_setCustomFont(connDriver, 4, data[1]);
	hd44780_hl_setCustomFont(connDriver, 5, data[0]);	
	

	while (1) {
		int idx;
		for (idx = 0; idx < 20 * 5; idx++) {
			uint8_t  pos = idx / 5;
			uint8_t sym = idx % 5;

			hd44780_hl_printChar(connDriver, 0, pos, sym + 1);

			_delay_ms(15);
		}

		for (idx = (20 * 5) - 1; idx >= 0; idx--) {
			uint8_t  pos = idx / 5;
			uint8_t sym = idx % 5;

			hd44780_hl_printChar(connDriver, 0, pos, sym);		

			_delay_ms(15);
		}
	}

//	hd44780_hl_clear(connDriver);

    return 0;
}
