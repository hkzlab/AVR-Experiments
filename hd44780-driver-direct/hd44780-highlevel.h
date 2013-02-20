#include <stdint.h>

typedef enum {
	PVC160101Q_16x1 = 0,
	TMBC20464BSP_20x4 = 1
} hd44780_type;

typedef struct {
	void *conn_struct;
	uint8_t (*initialize)(void *cCstruct);
	void (*sendCommand)(void *cStruct, uint16_t command);
	hd44780_type type;
} hd44780_driver;

hd44780_driver* hd44780_hl_createDriver(hd44780_type lcd_type, void *conn_struct, uint8_t (*initialize)(void*), void (*sendCommand)(void*, uint16_t));
void hd44780_hl_init(hd44780_driver *driver, uint8_t show_cursor, uint8_t blink_cursor);
void hd44780_hl_printText(hd44780_driver *driver, uint8_t line, uint8_t position, char *text);
void hd44780_hl_printChar(hd44780_driver *driver, uint8_t line, uint8_t position, char c);
void hd44780_hl_shiftDisplay(hd44780_driver *driver, uint8_t direction); // direction: 0 -> left, 1 -> right
void hd44780_hl_shiftCursor(hd44780_driver *driver, uint8_t direction); // direction: 0 -> left, 1 -> right
void hd44780_hl_setCustomFont(hd44780_driver *driver, uint8_t slot, uint8_t *data);
void hd44780_hl_clear(hd44780_driver *driver);

