#ifndef _AVR_PS2_KEYB_HEADER_
#define _AVR_PS2_KEYB_HEADER_

#include <stdint.h>

// Clock port MUST be the one corresponding to INT0 !
// Data port can be set at will
void ps2keyb_init(volatile uint8_t *clockPort, volatile uint8_t *dataPort);

#endif /* _AVR_PS2_KEYB_HEADER_ */
