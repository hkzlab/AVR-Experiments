 
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

	_delay_ms(3000);

	hd44780_hl_init(connDriver, 0, 0);

	uint8_t data[] = {0x1B, 
					  0x1B,
					  0x00,
					  0x04,
					  0x04,
					  0x04,
					  0x11,
					  0x0E};
		

	hd44780_hl_setCustomFont(connDriver, 1, data);
	hd44780_hl_printText(connDriver, 0, 0, "Sopra la panca la capra campa sotto la panca la capra crepa");	

	_delay_ms(20000);

	hd44780_hl_clear(connDriver);

	while (1) {
	}
   
    return 0;
}
