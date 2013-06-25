#ifndef _AVR_EXPERIMENTS_DEFINES_
#define _AVR_EXPERIMENTS_DEFINES_

#if defined (__AVR_ATmega128__) // Apparently, devs forgot to add these...
// UBRR0 has split registers
#define UBRR1 _SFR_MEM16(0x99)
#endif

#endif /* _AVR_EXPERIMENTS_DEFINES_ */
