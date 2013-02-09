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

#define SELECT_MCP (PORTB &= ~(1<<PB4))
#define DESELECT_MCP (PORTB |= (1<<PB4))

int main(void) {    

	uint8_t cmd_buf[2];

    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	DDRA = 0xFF;
	DDRC = 0xFF;

	PORTA = 0x00;
	PORTC = 0x00;

	_delay_ms(6000);

	DESELECT_MCP;
	fprintf(stdout, "Starting up... and configuring SPI.\n");
	setup_spi(SPI_MODE_0, SPI_MSB, SPI_NO_INTERRUPT, SPI_MSTR_CLK4);
	fprintf(stdout, "SPI configured...\n");

	fprintf(stdout, "Setting up IOCON register!\n");
	buildMCPRegisterCommand(cmd_buf, 0, 0, IOCONA);
	SELECT_MCP;
	send_spi(cmd_buf[0]);
	send_spi(cmd_buf[1]);
	send_spi(0x20);
	DESELECT_MCP;
	_delay_ms(1000);

	fprintf(stdout, "Setting up IODIRA register!\n");
	buildMCPRegisterCommand(cmd_buf, 0, 0, IODIRA);
	SELECT_MCP;
	send_spi(cmd_buf[0]);
	send_spi(cmd_buf[1]);
	send_spi(0xFF);
	DESELECT_MCP;
	_delay_ms(1000);

	fprintf(stdout, "Setting up IODIRB register!\n");
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
		PORTA = (address >> 8) & 0x00FF;
		PORTC = (address >> 0) & 0x00FF;

		_delay_ms(5);

		buildMCPRegisterCommand(cmd_buf, 0, 1, GPIOA);
		SELECT_MCP;
		send_spi(cmd_buf[0]);
		send_spi(cmd_buf[1]);
		gpioa = send_spi(0x00);
		DESELECT_MCP;

		buildMCPRegisterCommand(cmd_buf, 0, 1, GPIOB);
		SELECT_MCP;
		send_spi(cmd_buf[0]);
		send_spi(cmd_buf[1]);
		gpiob = send_spi(0x00);
		DESELECT_MCP;

		fprintf(stdout, "0x%.4x : 0x%.2X 0x%.2X\n", address, gpioa, gpiob);

		if (address == 0xFFFF) break;
		else address++;
	}
   
	fprintf(stdout, ".DONE.\n");

    return 0;

}
