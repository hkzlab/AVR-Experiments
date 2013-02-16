#include "hd44780-commands.h"

uint16_t hd44780_ClearDisplay(void) {
	return 0x0001;
}

uint16_t hd44780_CursorHome(void) {
	return 0x0002;
}

uint16_t hd44780_EntryModeSet(uint8_t dir, uint8_t shift) {
	return (shift & 0x01) | ((dir & 0x01) << 1);
}

uint16_t hd44780_DisplayControl(uint8_t disp_ena, uint8_t cshow, uint8_t cblink) {
	return 0x08 | ((disp_ena & 0x01) << 2) | ((cshow & 0x01) << 1) | (cblink & 0x01);
}

uint16_t hd44780_SetShift(uint8_t sel, uint8_t dir) {
	return 0x10 | ((sel & 0x01) << 3) | ((dir & 0x01) << 2);
}

uint16_t hd44780_FunctionSet(uint8_t dataLen, uint8_t dLines, uint8_t cFont) {
	return 0x20 | ((dataLen & 0x01) << 4) | ((dLines & 0x01) << 3) | ((cFont & 0x01) << 2);
}

uint16_t hd44780_SetCGRAMAddr(uint8_t address) {
	return 0x40 | (address & 0x3F);
}

uint16_t hd44780_SetDDRAMAddr(uint8_t address) {
	return 0x80 | (address & 0x7F);
}
uint16_t hd44780_ReadBF_Addr(uint8_t bf, uint8_t address) {
	return 0x100 | ((bf & 0x01) << 7) | (address & 0x7F);
}

uint16_t hd44780_WriteData(uint8_t data) {
	return 0x200 | data;
}

uint16_t hd44780_WriteData(uint8_t data) {
	return 0x300 | data;
}

