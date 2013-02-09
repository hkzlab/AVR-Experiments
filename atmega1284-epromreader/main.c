/*
 * Test how to use uart stuff as an outside library. Redirects stdout
 * to uart.
 * 
 * Led at Arduino digital 13 (PORTB5). Cause we all love blinken leds!
 *
 * To compile and upload run: make clean; make; make program;
 * Connect to serial with: screen /dev/tty.usbserial-* 9600
 *
 * Copyright 2011 Mika Tuupola
 *
 * Licensed under the MIT license:
 *   http://www.opensource.org/licenses/mit-license.php
 *
 */
 
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "main.h"
#include "uart.h"

#include "spi.h"
#include "mcp23s17.h"

// When using SPI, a chip is selected/deselected by toggling
// a line connected to the CS# pin of the chip.
#define SELECT_MCP (PORTB &= ~(1<<PB4))
#define DESELECT_MCP (PORTB |= (1<<PB4))

int main(void) {    

	uint8_t cmd_buf[2];

	// Initialize serial port for output
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	// Set port A and port C of the MCU as output
	DDRA = 0xFF;
	DDRC = 0xFF;

	// Start with address 0
	PORTA = 0x00;
	PORTC = 0x00;

	_delay_ms(6000);

	// Configuring SPI...
	DESELECT_MCP; // Disable the MCP23S17 chip before starting the configuration
	setup_spi(SPI_MODE_0, SPI_MSB, SPI_NO_INTERRUPT, SPI_MSTR_CLK4);

	// Setup the IOCONA register in the MCP23S17: refer to the datasheet for full documentation
	buildMCPRegisterCommand(cmd_buf, 0, 0, IOCONA);
	SELECT_MCP; // Enable the MCP 
	send_spi(cmd_buf[0]);
	send_spi(cmd_buf[1]);
	send_spi(0x20); // This command disables the SEQOP mode so the address pointer of the MCP
					// does't increment automatically at each byte transfer
	DESELECT_MCP;
	_delay_ms(1000);

	// Setup IODIRA register to set all pins on port A as input
	buildMCPRegisterCommand(cmd_buf, 0, 0, IODIRA);
	SELECT_MCP;
	send_spi(cmd_buf[0]);
	send_spi(cmd_buf[1]);
	send_spi(0xFF); // All pins as input!
	DESELECT_MCP;
	_delay_ms(1000);

	// Do the same for IODIRB	
	buildMCPRegisterCommand(cmd_buf, 0, 0, IODIRB);
	SELECT_MCP;
	send_spi(cmd_buf[0]);
	send_spi(cmd_buf[1]);
	send_spi(0xFF);
	DESELECT_MCP;
	_delay_ms(1000);

	uint8_t gpioa, gpiob;
	uint16_t address = 0x0000;
	
	while (1) {
		// Toggle pins on port A and C to compose the address 
		PORTA = (address >> 8) & 0x00FF;
		PORTC = (address >> 0) & 0x00FF;

		_delay_ms(5);

		// Read first byte on port A
		buildMCPRegisterCommand(cmd_buf, 0, 1, GPIOA);
		SELECT_MCP;
		send_spi(cmd_buf[0]);
		send_spi(cmd_buf[1]);
		gpioa = send_spi(0x00);
		DESELECT_MCP;

		// Read second byte on port B
		buildMCPRegisterCommand(cmd_buf, 0, 1, GPIOB);
		SELECT_MCP;
		send_spi(cmd_buf[0]);
		send_spi(cmd_buf[1]);
		gpiob = send_spi(0x00);
		DESELECT_MCP;

		// Print the data to serial port in this format -> address : byte1 byte2
		fprintf(stdout, "0x%.4x : 0x%.2X 0x%.2X\n", address, gpioa, gpiob);

		if (address == 0xFFFF) break; // Loop until we reach the last address!
		else address++;
	}
   
    return 0;
}
