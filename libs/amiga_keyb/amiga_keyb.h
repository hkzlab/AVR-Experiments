#ifndef _AMIGA_KEYBOARD_HEADER_
#define _AMIGA_KEYBOARD_HEADER_

#include <stdint.h>

void amikbd_setup(volatile uint8_t *clockPort, volatile uint8_t *clockDir, uint8_t clockPNum);
void amikbd_init(void);

#endif /* _AMIGA_KEYBOARD_HEADER_ */
