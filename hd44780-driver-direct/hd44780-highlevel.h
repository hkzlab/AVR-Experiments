#include <stdint.h>

typedef struct {
	void *conn_struct;
	void (*initialize)(void *cCstruct);
	void (*sendCommand)(void *cStruct, uint16_t command);
} hd44780_driver;

