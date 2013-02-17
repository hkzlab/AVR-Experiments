 
#ifndef F_CPU
#define F_CPU 8000000UL // MCU Frequency set to 8Mhz
#endif

#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "hd44780-avr-interface.h"
#include "hd44780-commands.h"

#include "main.h"
#include "uart.h"

int main(void) {    
	hd44780_connection conn;
	uint16_t lcd_command;

	DDRB = 0xFF; // Set port B as output!
	PORTB = 0x00;

	conn.dataPort = &PORTB;
	conn.dataPinsBlock = 1;

	conn.rsPort = &PORTB;
	conn.rsPin = 0;

	conn.enPort = &PORTB;
	conn.enPin = 1;

	// Initialize serial port for output
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	_delay_ms(3000);

	hd44780_initLCD4Bit(&conn);	

	lcd_command = hd44780_FunctionSet(0, 1, 0);
	hd44780_sendCommand(&conn, lcd_command);
	fprintf(stdout, "Sent %.4X\n", lcd_command);
	_delay_ms(5);

	lcd_command = hd44780_DisplayControl(0, 0, 0);
	hd44780_sendCommand(&conn, lcd_command);
	fprintf(stdout, "Sent %.4X\n", lcd_command);	
	_delay_ms(5);	

	lcd_command = hd44780_ClearDisplay();
	hd44780_sendCommand(&conn, lcd_command);
	fprintf(stdout, "Sent %.4X\n", lcd_command);	
	_delay_ms(5);

	lcd_command = hd44780_EntryModeSet(1, 0);
	hd44780_sendCommand(&conn, lcd_command);
	fprintf(stdout, "Sent %.4X\n", lcd_command);	
	_delay_ms(5);

	lcd_command = hd44780_DisplayControl(1, 1, 1);
	hd44780_sendCommand(&conn, lcd_command);
	fprintf(stdout, "Sent %.4X\n", lcd_command);	
	_delay_ms(5);	

	fprintf(stdout, "DONE!!!\n");

	while (1) {
		//fprintf(stdout, "Loop!\n");
		_delay_ms(5);
	}
   
    return 0;
}
