#ifndef _MCP23S17_HEADER_
#define _MCP23S17_HEADER_

#include <stdint.h>

typedef enum {
	IOCONA = 0x0A,		// Config. register
	IOCONB = 0x0B,	
	IODIRA = 0x00,		// I/O direction 1=IN
	IODIRB = 0x01,
	IOPOLA = 0x02,		// I/O polarity 1=Inverted
	IOPOLB = 0x03,
	GPIOA = 0x12,		// GPIO registers
	GPIOB = 0x13,
	GPINTENA = 0x04,	// IOC Enable
	GPINTENB = 0x05,
	INTCONA = 0x08, 	// Interrupt Cont. 1=Compare to defval 0=change
	INTCONB = 0x09,
	DEFVALA = 0x06,		// IOC Default value
	DEFVALB = 0x07,
	GPPUA = 0x0C,		// Weak Pull-up 1=pull high via 100k ohm
	GPPUB = 0x0D,
	OLATA = 0x14,
	OLATB = 0x15,
	INTFA = 0x0E,
	INTFB = 0x0F,
	INTCAPA = 0x10,
	INTCAPB = 0x11
} MCP_Register;

void buildMCPRegisterCommand(uint8_t *buf, uint8_t address, uint8_t read, MCP_Register reg);

#endif /* _MCP23S17_HEADER_ */
