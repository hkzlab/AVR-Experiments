#include <stdint.h>

// The descriptions are taken from here: http://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller

// Clears the display and returns cursor to the home position (address 0)
uint16_t hd44780_ClearDisplay(void);
// Returns cursor to home position, also returns shifted display to original position. DRAM contents remain unchanged.
uint16_t hd44780_CursorHome(void);
// Sets cursor move direction (dir) and specifies display shift. The operation is performed during data r/w
uint16_t hd44780_EntryModeSet(uint8_t dir, uint8_t shift);
// Enables/disables whole display, enables/disables showing of cursor, enables/disables cursor blink.
uint16_t hd44780_DisplayControl(uint8_t disp_ena, uint8_t cshow, uint8_t cblink);
// Selects cursor move or display shift, and selects direction. DDRAM contents unchanged
uint16_t hd44780_SetShift(uint8_t sel, uint8_t dir);
// Selects data length for display interface, number of display lines and char fonts
uint16_t hd44780_FunctionSet(uint8_t dataLen, uint8_t dLines, uint8_t cFont);
// Sets CGRAM address, CGRAM data are sent and received after this setting
uint16_t hd44780_SetCGRAMAddr(uint8_t address);
// Sets DDRAM address, CGRAM data are sent and received after this setting
uint16_t hd44780_SetDDRAMAddr(uint8_t address);
// Reads busy flag indicating internal op being performed and reads CGRAM/DDRAM address (depends on previous op)
uint16_t hd44780_ReadBF_Addr(uint8_t bf, uint8_t address);
// Writes data to CGRAM/DDRAM address, depending on previous set op
uint16_t hd44780_WriteData(uint8_t data);
// Read data from CGRAM/DDRAM
uint16_t hd44780_WriteData(uint8_t data); // data?

