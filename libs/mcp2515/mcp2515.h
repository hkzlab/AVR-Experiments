#ifndef _MCP2515_HEADER_
#define _MCP2515_HEADER_

#include <avr/io.h>

#ifdef __cplusplus
extern "C"{
#endif

/** COMMAND **/
#define MCP2515_INSTR_RESET		0xC0
#define MCP2515_INSTR_READ		0x03
#define MCP2515_INSTR_WRITE		0x02
#define MCP2515_INSTR_READRX	0x90
#define MCP2515_INSTR_LOADTX	0x40
#define MCP2515_INSTR_RTS		0x80
#define MCP2515_INSTR_READSTAT	0xA0
#define MCP2515_INSTR_RXSTAT	0xB0
#define MCP2515_INSTR_BITMOD	0x05

/** REGISTERS **/
// Register map of the MCP2515
#define MCP2515_REG_RXF0SIDH	0x00
#define MCP2515_REG_RXF0SIDL	0x01
#define MCP2515_REG_RXF0EID8	0x02
#define MCP2515_REG_RXF0EID0	0x03
#define MCP2515_REG_RXF1SIDH	0x04
#define MCP2515_REG_RXF1SIDL	0x05
#define MCP2515_REG_RXF1EID8	0x06
#define MCP2515_REG_RXF1EID0	0x07
#define MCP2515_REG_RXF2SIDH	0x08
#define MCP2515_REG_RXF2SIDL	0x09
#define MCP2515_REG_RXF2EID8	0x0A
#define MCP2515_REG_RXF2EID0	0x0B
#define MCP2515_REG_BFPCTRL		0x0C
#define MCP2515_REG_TXRTSCTRL	0x0D
#define MCP2515_REG_CANSTAT		0x0E
#define MCP2515_REG_CANCTRL		0x0F

#define MCP2515_REG_RXF3SIDH	0x10
#define MCP2515_REG_RXF3SIDL	0x11
#define MCP2515_REG_RXF3EID8	0x12
#define MCP2515_REG_RXF3EID0	0x13
#define MCP2515_REG_RXF4SIDH	0x14
#define MCP2515_REG_RXF4SIDL	0x15
#define MCP2515_REG_RXF4EID8	0x16
#define MCP2515_REG_RXF4EID0	0x17
#define MCP2515_REG_RXF5SIDH	0x18
#define MCP2515_REG_RXF5SIDL	0x19
#define MCP2515_REG_RXF5EID8	0x1A
#define MCP2515_REG_RXF5EID0	0x1B
#define MCP2515_REG_TEC			0x1C
#define MCP2515_REG_REC         0x1D
#define MCP2515_REG_CANSTAT1	0x1E
#define MCP2515_REG_CANCTRL1	0x1F

#define MCP2515_REG_RXM0SIDH	0x20
#define MCP2515_REG_RXM0SIDL	0x21
#define MCP2515_REG_RXM0EID8	0x22
#define MCP2515_REG_RXM0EID0	0x23
#define MCP2515_REG_RXM1SIDH	0x24
#define MCP2515_REG_RXM1SIDL	0x25
#define MCP2515_REG_RXM1EID8	0x26
#define MCP2515_REG_RXM1EID0	0x27
#define MCP2515_REG_CNF3		0x28
#define MCP2515_REG_CNF2		0x29
#define MCP2515_REG_CNF1		0x2A
#define MCP2515_REG_CANINTE		0x2B
#define MCP2515_REG_CANINTF		0x2C
#define MCP2515_REG_EFLG		0x2D
#define MCP2515_REG_CANSTAT2	0x2E
#define MCP2515_REG_CANCTRL2	0x2F

// TXB0
#define MCP2515_REG_TXB0CTRL	0x30
#define MCP2515_REG_TXB0SIDH	0x31
#define MCP2515_REG_TXB0SIDL	0x32
#define MCP2515_REG_TXB0EID8	0x33
#define MCP2515_REG_TXB0EID0	0x34
#define MCP2515_REG_TXB0DLC		0x35
#define MCP2515_REG_TXB0D0		0x36
#define MCP2515_REG_TXB0D1		0x37
#define MCP2515_REG_TXB0D2		0x38
#define MCP2515_REG_TXB0D3		0x39
#define MCP2515_REG_TXB0D4		0x3A
#define MCP2515_REG_TXB0D5		0x3B
#define MCP2515_REG_TXB0D6		0x3C
#define MCP2515_REG_TXB0D7		0x3D
#define MCP2515_REG_CANSTAT3	0x3E
#define MCP2515_REG_CANCTRL3	0x3F

// TXB1
/*
#define MCP2515_REG_TXB1CTRL	0x40
#define MCP2515_REG_TXB1SIDH	0x41
#define MCP2515_REG_TXB1SIDL	0x42
#define MCP2515_REG_TXB1EID8	0x43
#define MCP2515_REG_TXB1EID0	0x44
#define MCP2515_REG_TXB1DLC		0x45
#define MCP2515_REG_TXB1D0		0x46
#define MCP2515_REG_TXB1D1		0x47
#define MCP2515_REG_TXB1D2		0x48
#define MCP2515_REG_TXB1D3		0x49
#define MCP2515_REG_TXB1D4		0x4A
#define MCP2515_REG_TXB1D5		0x4B
#define MCP2515_REG_TXB1D6		0x4C
#define MCP2515_REG_TXB1D7		0x4D
#define MCP2515_REG_CANSTAT4	0x4E
#define MCP2515_REG_CANCTRL4	0x4F

// TXB2
#define MCP2515_REG_TXB2CTRL	0x50
#define MCP2515_REG_TXB2SIDH	0x51
#define MCP2515_REG_TXB2SIDL	0x52
#define MCP2515_REG_TXB2EID8	0x53
#define MCP2515_REG_TXB2EID0	0x54
#define MCP2515_REG_TXB2DLC		0x55
#define MCP2515_REG_TXB2D0		0x56
#define MCP2515_REG_TXB2D1		0x57
#define MCP2515_REG_TXB2D2		0x58
#define MCP2515_REG_TXB2D3		0x59
#define MCP2515_REG_TXB2D4		0x5A
#define MCP2515_REG_TXB2D5		0x5B
#define MCP2515_REG_TXB2D6		0x5C
#define MCP2515_REG_TXB2D7		0x5D
#define MCP2515_REG_CANSTAT5	0x5E
#define MCP2515_REG_CANCTRL5	0x5F
*/

// RXB0
#define MCP2515_REG_RXB0CTRL	0x60
#define MCP2515_REG_RXB0SIDH	0x61
#define MCP2515_REG_RXB0SIDL	0x62
#define MCP2515_REG_RXB0EID8	0x63
#define MCP2515_REG_RXB0EID0	0x64
#define MCP2515_REG_RXB0DLC		0x65
#define MCP2515_REG_RXB0D0		0x66
#define MCP2515_REG_RXB0D1		0x67
#define MCP2515_REG_RXB0D2		0x68
#define MCP2515_REG_RXB0D3		0x69
#define MCP2515_REG_RXB0D4		0x6A
#define MCP2515_REG_RXB0D5		0x6B
#define MCP2515_REG_RXB0D6		0x6C
#define MCP2515_REG_RXB0D7		0x6D
#define MCP2515_REG_CANSTAT6	0x6E
#define MCP2515_REG_CANCTRL6	0x6F

// RXB1
/*
#define MCP2515_REG_RXB1CTRL	0x70
#define MCP2515_REG_RXB1SIDH	0x71
#define MCP2515_REG_RXB1SIDL	0x72
#define MCP2515_REG_RXB1EID8	0x73
#define MCP2515_REG_RXB1EID0	0x74
#define MCP2515_REG_RXB1DLC		0x75
#define MCP2515_REG_RXB1D0		0x76
#define MCP2515_REG_RXB1D1		0x77
#define MCP2515_REG_RXB1D2		0x78
#define MCP2515_REG_RXB1D3		0x79
#define MCP2515_REG_RXB1D4		0x7A
#define MCP2515_REG_RXB1D5		0x7B
#define MCP2515_REG_RXB1D6		0x7C
#define MCP2515_REG_RXB1D7		0x7D
#define MCP2515_REG_CANSTAT7	0x7E
#define MCP2515_REG_CANCTRL7	0x7F
*/
/************/

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

typedef enum {
	mcp_func_normal = 0x00,
	mcp_func_sleep = 0x02,
	mcp_func_loopback = 0x40,
	mcp_func_listen = 0x60,
	mcp_func_config = 0x80
} mcp2515_func_mode;

// Assuming Fosc = 20Mhz
typedef enum {
	mcp_can_speed_50 = 0x00,
	mcp_can_speed_125 = 0x01,
	mcp_can_speed_250 = 0x02,
	mcp_can_speed_500 = 0x03
} mcp2515_canspeed;

typedef enum {
	mcp_tx_txb0 = 0x00,
	mcp_tx_txb1 = 0x10,
	mcp_tx_txb2 = 0x20
} mcp2515_txb;

typedef enum {
	mcp_tx_rxb0 = 0x00,
	mcp_tx_rxb1 = 0x10
} mcp2515_rxb;

void mcp2515_simpleStartup(mcp2515_canspeed speed, uint8_t loopback);
void mcp2515_setupRX(mcp2515_rxb rxb, const uint8_t *filter, const uint8_t *mask);
void mcp2515_setupTX(mcp2515_txb txb, const uint8_t *addr, uint8_t dLen, uint8_t rtr);
uint8_t mcp2515_setCanSpeed(mcp2515_canspeed speed);
void mcp2515_setMode(mcp2515_func_mode mode);

/* TIMING:
 * CNF1:
 *	- bit 7-6 -> SJW: Synchronization Jump Width Length bits <1:0>
 *		11: Length = 4 * Tq
 *		10: Length = 3 * Tq
 *		01: Length = 2 * Tq
 *		00: Length = 1 * Tq
 *	- bit 5-0 -> BRP: Baud Rate Prescaler bits <5:0>
 *		Tq = 2 * (BRP + 1)/Fosc
 *
 * CNF2:
 *	- bit 7 -> BTLMODE: PS2 Bit Time Length
 *		1: Length of PS2 determined by PHSEG22:PHSEG20 bits of CNF3
 *		0: Length of PS2 is the greater of PS1 and IPT (2 Tq)
 *	- bit 6 -> SAM: Sample Point Configuration
 *		1: Bus line is sampled three times at the sample point
 *		0: Bus line is sampled once at the sample point
 *	- bit 5-3 -> PHSEG1: PS1 Length bits <2:0>
 *		(PHSEG1 + 1) * Tq
 *	- bit 2-0 -> PRSEG: Propagation Segment LEngth bits <2:0>
 *		(PRSEG + 1) * Tq
 *
 * CNF3:
 *	- bit 7 -> SOF: Start-of-Frame signal
 *		If CANCTRL.CLKEN = 1
 *			1: CLKOUT pin enabled for SOF signal
 *			0: CLKOUT pin enabled for clockout function
 *		If CANCTRL.CLKEN = 0, Bit is "don't care"
 *	- bit 6 -> WAKFIL: Wake-up Filter bit
 *		1: Wake-up filter enabled
 *		0: Wake-up filter disabled
 *	- bit 5-3 -> UNIMPLEMENTED
 *	- bit 2-0 -> PHSEG2: PS2 Length bits <2:0>
 *		(PHSEG2 + 1) * Tq 
 *		NOTE: Minimum valid setting for PS2 is 2 Tq
 *
 */
uint8_t mcp2515_setBitTiming(uint8_t rCNF1, uint8_t rCNF2, uint8_t rCNF3);

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
