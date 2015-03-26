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

typedef enum {
	mcp_rts_tx0	= 0x01,
	mcp_rts_tx1 = 0x02,
	mcp_rts_tx2 = 0x04,
	mcp_rts_all = 0x07
} mcp2515_rts_buf;

typedef enum {
	mcp_read_tx0 = 0x00,
	mcp_read_tx1 = 0x04
} mcp2515_read_buf;

uint8_t mcp2515_readRegister(uint8_t address);
void mcp2515_writeRegister(uint8_t address, uint8_t value);
void mcp2515_bitModify(uint8_t address, uint8_t value, uint8_t mask);
uint8_t mcp2515_readStatus(void);
void mcp2515_rts(mcp2515_rts_buf buf);
uint8_t mcp2515_readRX(mcp2515_read_buf buf);
void mcp2515_reset(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _MCP2515_HEADER_ */
