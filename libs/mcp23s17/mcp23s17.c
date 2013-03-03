#include "mcp23s17.h"

#include <string.h>
#include <stdio.h>

void buildMCPRegisterCommand(uint8_t *buf, uint8_t address, uint8_t read, MCP_Register reg) {
	if (!buf) return;

	address &= 0x7;

	buf[0] = 0x40 | (address << 1) | (read & 0x1); // Device Opcode
	buf[1] = reg;

	//fprintf(stdout, "buf[0]=0x%.2X buf[1]=0x%.2X\n", buf[0], buf[1]);
}
