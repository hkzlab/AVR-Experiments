#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "spi.h"
#include "mcp2515.h"


#include "uart.h"
#include "main.h"

static mcp2515_canint_stat status;
static uint8_t errflag;

static volatile uint8_t interrupt_received = 0, first_pkt = 1;
void (*int_handler)(void); 

static void extInterruptINIT(void (*handler)(void));
static void interrupt_handler(void);

int main(void) {
	uint8_t raddress[] = {0x54, 0xEB, 0xFA, 0xB1, 0x00};
	uint8_t rexdata[] = {0xC0, 0xD1, 0x00, 0xC0, 0xD1, 0x00, 0xBE, 0xEF};
	
	uint8_t address[] = {0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t exdata[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t psize;

	srand(1);

	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	extInterruptINIT(interrupt_handler);
	sei(); // Enable interrupts
	
	// Setup SPI
	setup_spi(SPI_MODE_0, SPI_MSB, SPI_NO_INTERRUPT, SPI_MSTR_CLK2);

	mcp2515_simpleStartup(mcp_can_speed_25, 0);

	wdt_enable(WDTO_500MS);

	mcp2515_setupTX(mcp_tx_txb0, raddress, 8, 0, 0);

	interrupt_received = 1;
	while (1) {
		cli();

		fprintf(stdout, "ERR %.2X\n", errflag);

		if(interrupt_received) {
			if (status.rx1if) {
				psize = mcp2515_readMSG(mcp_rx_rxb1, address, exdata);
				fprintf(stdout, "\tRXB1 (%u) -> %.2X %.2X %.2X %.2X | %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X\n", psize, address[0], address[1], address[2], address[3], exdata[0], exdata[1], exdata[2], exdata[3], exdata[4], exdata[5], exdata[6], exdata[7]);
			}

			if (status.rx0if) {
				psize = mcp2515_readMSG(mcp_rx_rxb0, address, exdata);
				fprintf(stdout, "\tRXB0 (%u) -> %.2X %.2X %.2X %.2X | %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X\n", psize, address[0], address[1], address[2], address[3], exdata[0], exdata[1], exdata[2], exdata[3], exdata[4], exdata[5], exdata[6], exdata[7]);
			}

			interrupt_received = 0;
		} 

		if(!(rand()%2)) {
			mcp2515_loadMSG(mcp_tx_txb0, rexdata, 8);
			mcp2515_sendMSG(RTS_TXB0);
			while(!(mcp2515_readRegister(MCP2515_REG_CANINTF) & 0x04));
		}

		mcp2515_bitModify(MCP2515_REG_EFLG, 0x00, 0xC0); // Clear error flags
		
		sei();

		wdt_reset();
		
		_delay_ms(100);
	}

	wdt_disable();

    return 0;
}

static void extInterruptINIT(void (*handler)(void)) {
	/* Set function pointer */
	int_handler = handler;
	/* Initialize external interrupt on pin INT0 on failing edge */
	EICRA |= (1 << ISC01);
	EIMSK |= (1 << INT0);
}

static void interrupt_handler(void) {
	interrupt_received = 1;
	status = mcp2515_intStatus(); // Read interrupt
	mcp2515_writeRegister(MCP2515_REG_CANINTF, 0x00); // Clear interrupt flags
	errflag = mcp2515_readRegister(MCP2515_REG_EFLG);
}

/* System interrupt handler */
SIGNAL(INT0_vect) {
	int_handler();
} 




