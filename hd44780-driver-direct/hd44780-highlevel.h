#include <stdint.h>

typedef enum {
	hd44780_16x1 = 0
} hd44780_type;

typedef struct {
	void *conn_struct;
	uint8_t (*initialize)(void *cCstruct);
	void (*sendCommand)(void *cStruct, uint16_t command);
	hd44780_type type;
} hd44780_driver;

void hd44780_hl_init(hd44780_driver *driver, uint8_t show_cursor, uint8_t blink_cursor);
void hd44780_hl_printText(hd44780_driver *driver, uint8_t position, char *text);
