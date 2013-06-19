#ifndef _AVR_PS2_KEYB_HEADER_
#define _AVR_PS2_KEYB_HEADER_

#include <stdint.h>

// Clock port MUST be the one corresponding to INT0 !

// Data port can be set at will
void ps2keyb_init(volatile uint8_t *dataPort, volatile uint8_t *dataDir, volatile uint8_t *dataPin, uint8_t pNum);

#endif /* _AVR_PS2_KEYB_HEADER_ */
