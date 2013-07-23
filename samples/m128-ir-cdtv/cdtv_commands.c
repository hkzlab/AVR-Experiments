#include "cdtv_commands.h"

// Only the first 12 most important bits are meaningful
const uint16_t cdtv_ir_table[33] PROGMEM = {
	0x0010, // '1'
	0x0210, // '2'
	0x0110, // '3'
	0x0090, // '4'
	0x0290, // '5'
	0x0190, // '6'
	0x0050, // '7'
	0x0250, // '8'
	0x0150, // '9'
	0x0390, // '0'

	0x0310, // ESCAPE
	0x0350, // ENTER
	0x0220, // GENLOCK
	0x0020, // CD/TV
	0x0120, // POWER

	0x0320, // REW
	0x00A0, // PLAY/PAUSE
	0x01A0, // FAST-FORWARD
	0x02A0, // STOP
	0x0060, // VOL UP
	0x03A0, // VOL DOWN

	0x0800, // Mouse A
	0x0400, // Mouse B
	0x0200, // Mouse UP
	0x0040, // Mouse RIGHT
	0x0100, // Mouse DOWN
	0x0080, // Mouse LEFT

	0x8800, // Joy A
	0x8400, // Joy B
	0x8200, // Joy UP
	0x8040, // Joy RIGHT
	0x8100, // Joy DOWN
	0x8080  // Joy LEFT
};
