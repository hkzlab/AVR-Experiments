 
#ifndef F_CPU
#define F_CPU 8000000UL // MCU Frequency set to 8Mhz
#endif

#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#include "hd44780-avr-interface.h"
#include "hd44780-highlevel.h"

#include "main.h"
#include "uart.h"

int main(void) {    
	hd44780_driver connDriver;

	connDriver.conn_struct = (void*)malloc(sizeof(hd44780_connection));
	((hd44780_connection*)(connDriver.conn_struct))->dataPort = &PORTB;
	((hd44780_connection*)(connDriver.conn_struct))->dataPinsBlock = 1;
	((hd44780_connection*)(connDriver.conn_struct))->rsPort = &PORTB;
	((hd44780_connection*)(connDriver.conn_struct))->rsPin = 0;
	((hd44780_connection*)(connDriver.conn_struct))->enPort = &PORTB;
	((hd44780_connection*)(connDriver.conn_struct))->enPin = 1;

	connDriver.initialize = (uint8_t (*)(void*))hd44780_initLCD4Bit;
	connDriver.sendCommand = (void (*)(void*, uint16_t))hd44780_sendCommand;

	connDriver.type = hd44780_16x1;

	DDRB = 0xFF; // Set port B as output!
	PORTB = 0x00; // And zero it

	// Initialize serial port for output
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	_delay_ms(3000);

	hd44780_hl_init(&connDriver, 0, 0);

	hd44780_hl_printText(&connDriver, 4, "Ciao Fabio!");
	
	fprintf(stdout, "DONE!!!\n");

	while (1) {
		//fprintf(stdout, "Loop!\n");
		_delay_ms(5);
	}
   
    return 0;
}
