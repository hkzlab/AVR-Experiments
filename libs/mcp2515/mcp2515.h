#ifndef _MCP2515_HEADER_
#define _MCP2515_HEADER_

#include <avr/io.h>

#ifdef __cplusplus
extern "C"{
#endif

#define MCP2515_INSTR_RESET		0xC0
#define MCP2515_INSTR_READ		0x03
#define MCP2515_INSTR_WRITE		0x02
#define MCP2515_INSTR_READRX	0x90
#define MCP2515_INSTR_LOADTX	0x40
#define MCP2515_INSTR_RTS		0x80
#define MCP2515_INSTR_READSTAT	0xA0
#define MCP2515_INSTR_RXSTAT	0xB0
#define MCP2515_INSTR_BITMOD	0x05

uint8_t mcp2515_readRegister(uint8_t address);
void mcp2515_writeRegister(uint8_t address, uint8_t value);
void mcp2515_reset(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _MCP2515_HEADER_ */
