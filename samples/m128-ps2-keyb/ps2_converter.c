#include "ps2_converter.h"

#include <stdio.h>

#include <avr/pgmspace.h>

// PS2 scancodes
// http://www.computer-engineering.org/ps2keyboard/scancodes2.html

// Amiga scancodes
// http://lxr.free-electrons.com/source/drivers/input/keyboard/amikbd.c

const uint8_t ps2_normal_convtable[256] PROGMEM = {
	0x00, // 00 
	0x58, // 01 - F9
	0x00, // 02 
	0x54, // 03 - F5
	0x52, // 04 - F3
	0x50, // 05 - F1
	0x51, // 06 - F2
	0x00, // 07 - F12 --- Not present in Amiga
	0x00, // 08
	0x59, // 09 - F10
	0x57, // 0A - F8
	0x55, // 0B - F6
	0x53, // 0C - F4
	0x42, // 0D - 'TAB'
	0x00, // 0E - '`'
	0x00, // 0F
	0x00, // 10
	0x64, // 11 - 'LEFT ALT'
	0x60, // 12 - 'LEFT SHIFT'
	0x00, // 13
	0x00, // 14 - 'LEFT CTRL'
	0x00, // 15 - 'Q'
	0x00, // 16 - '1'
	0x00, // 17
	0x00, // 18
	0x00, // 19
	0x00, // 1A - 'Z'
	0x00, // 1B - 'S'
	0x00, // 1C - 'A'
	0x00, // 1D - 'W'
	0x00, // 1E - '2'
	0x00, // 1F
	0x00, // 20
	0x00, // 21 - 'C'
	0x00, // 22 - 'X'
	0x00, // 23 - 'D'
	0x00, // 24 - 'E'
	0x00, // 25 - '4'
	0x00, // 26 - '3'
	0x00, // 27
	0x00, // 28
	0x00, // 29 - 'SPACE'
	0x00, // 2A - 'V'
	0x23, // 2B - 'F'
	0x00, // 2C - 'T'
	0x00, // 2D - 'R'
	0x00, // 2E - '5'
	0x00, // 2F
	0x00, // 30
	0x00, // 31 - 'N'
	0x00, // 32 - 'B'
	0x00, // 33 - 'H'
	0x00, // 34 - 'G'
	0x00, // 35 - 'Y'
	0x00, // 36 - '6'
	0x00, // 37
	0x00, // 38
	0x00, // 39 
	0x00, // 3A - 'M'
	0x00, // 3B - 'J'
	0x00, // 3C - 'U' 
	0x00, // 3D - '7'
	0x00, // 3E - '8'
	0x00, // 3F
	0x00, // 40
	0x00, // 41 - ','
	0x00, // 42 - 'K'
	0x00, // 43 - 'I'
	0x00, // 44 - 'O'
	0x00, // 45 - '0'
	0x00, // 46 - '9'
	0x00, // 47
	0x00, // 48
	0x00, // 49 - '.'
	0x00, // 4A - '/'
	0x00, // 4B - 'L'
	0x00, // 4C - ';'
	0x00, // 4D - 'P'
	0x00, // 4E - '-'
	0x00, // 4F
	0x00, // 50
	0x00, // 51
	0x00, // 52 - '
	0x00, // 53
	0x00, // 54 - '['
	0x00, // 55 - '='
	0x00, // 56
	0x00, // 57
	0x00, // 58 - 'CAPS'
	0x00, // 59 - 'RIGHT SHIFT'
	0x00, // 5A - 'ENTER'
	0x00, // 5B - ']'
	0x00, // 5C
	0x00, // 5D - '\'
	0x00, // 5E
	0x00, // 5F
	0x00, // 60
	0x00, // 61
	0x00, // 62
	0x00, // 63
	0x00, // 64
	0x00, // 65
	0x00, // 66 - 'BACKSPACE'
	0x00, // 67
	0x00, // 68
	0x00, // 69 - 'KP 1'
	0x00, // 6A
	0x00, // 6B - 'KP 4'
	0x00, // 6C - 'KP 7'
	0x00, // 6D
	0x00, // 6E
	0x00, // 6F
	0x00, // 70 - 'KP 0'
	0x00, // 71 - 'KP .'
	0x00, // 72 - 'KP 2'
	0x00, // 73 - 'KP 5'
	0x00, // 74 - 'KP 6'
	0x00, // 75 - 'KP 8'
	0x45, // 76 - ESC
	0x00, // 77 - 'NUM' (Num lock???)
	0x00, // 78 - F11
	0x00, // 79 - 'KP +'
	0x00, // 7A - 'KP 3'
	0x00, // 7B - 'KP -'
	0x00, // 7C - 'KP *'
	0x00, // 7D - 'KP 9'
	0x00, // 7E - 'SCROLL LOCK' ?
	0x00, // 7F
	0x00, // 80
	0x00, // 81
	0x00, // 82
	0x00, // 83 - F7
	0x00, // 84
	0x00, // 85
	0x00, // 86
	0x00, // 87
	0x00, // 88
	0x00, // 89
	0x00, // 8A
	0x00, // 8B
	0x00, // 8C
	0x00, // 8D
	0x00, // 8E
	0x00, // 8F
	0x00, // 90
	0x00, // 91
	0x00, // 92
	0x00, // 93
	0x00, // 94
	0x00, // 95
	0x00, // 96
	0x00, // 97
	0x00, // 98
	0x00, // 99
	0x00, // 9A
	0x00, // 9B
	0x00, // 9C
	0x00, // 9D
	0x00, // 9E
	0x00, // 9F
	0x00, // A0
	0x00, // A1
	0x00, // A2
	0x00, // A3
	0x00, // A4
	0x00, // A5
	0x00, // A6
	0x00, // A7
	0x00, // A8
	0x00, // A9
	0x00, // AA
	0x00, // AB
	0x00, // AC
	0x00, // AD
	0x00, // AE
	0x00, // AF
	0x00, // B0
	0x00, // B1
	0x00, // B2
	0x00, // B3
	0x00, // B4
	0x00, // B5
	0x00, // B6
	0x00, // B7
	0x00, // B8
	0x00, // B9
	0x00, // BA
	0x00, // BB
	0x00, // BC
	0x00, // BD
	0x00, // BE
	0x00, // BF
	0x00, // C0
	0x00, // C1
	0x00, // C2
	0x00, // C3
	0x00, // C4
	0x00, // C5
	0x00, // C6
	0x00, // C7
	0x00, // C8
	0x00, // C9
	0x00, // CA
	0x00, // CB
	0x00, // CC
	0x00, // CD
	0x00, // CE
	0x00, // CF
	0x00, // D0
	0x00, // D1
	0x00, // D2
	0x00, // D3
	0x00, // D4
	0x00, // D5
	0x00, // D6
	0x00, // D7
	0x00, // D8
	0x00, // D9
	0x00, // DA
	0x00, // DB
	0x00, // DC
	0x00, // DD
	0x00, // DE
	0x00, // DF
	0x00, // E0
	0x00, // E1
	0x00, // E2
	0x00, // E3
	0x00, // E4
	0x00, // E5
	0x00, // E6
	0x00, // E7
	0x00, // E8
	0x00, // E9
	0x00, // EA
	0x00, // EB
	0x00, // EC
	0x00, // ED
	0x00, // EE
	0x00, // EF
	0x00, // F0
	0x00, // F1
	0x00, // F2
	0x00, // F3
	0x00, // F4
	0x00, // F5
	0x00, // F6
	0x00, // F7
	0x00, // F8
	0x00, // F9
	0x00, // FA
	0x00, // FB
	0x00, // FC
	0x00, // FD
	0x00, // FE
	0x00  // FF
};

const uint8_t ps2_extended_convtable[256] PROGMEM = {
	0x00, // 00 
	0x00, // 01
	0x00, // 02
	0x00, // 03
	0x00, // 04
	0x00, // 05
	0x00, // 06
	0x00, // 07
	0x00, // 08
	0x00, // 09
	0x00, // 0A
	0x00, // 0B
	0x00, // 0C
	0x00, // 0D
	0x00, // 0E
	0x00, // 0F
	0x00, // 10
	0x00, // 11 - 'RIGHT ALT'
	0x00, // 12
	0x00, // 13
	0x00, // 14 - 'RIGHT CTRL'
	0x00, // 15
	0x00, // 16
	0x00, // 17
	0x00, // 18
	0x00, // 19
	0x00, // 1A
	0x00, // 1B
	0x00, // 1C
	0x00, // 1D
	0x00, // 1E
	0x00, // 1F - 'LEFT GUI' (Windows button?)
	0x00, // 20
	0x00, // 21
	0x00, // 22
	0x00, // 23
	0x00, // 24
	0x00, // 25
	0x00, // 26
	0x00, // 27 - 'RIGHT GUI' (Windows button?)
	0x00, // 28
	0x00, // 29
	0x00, // 2A
	0x00, // 2B
	0x00, // 2C
	0x00, // 2D
	0x00, // 2E
	0x00, // 2F - 'APPS' ????
	0x00, // 30
	0x00, // 31
	0x00, // 32
	0x00, // 33
	0x00, // 34
	0x00, // 35
	0x00, // 36
	0x00, // 37
	0x00, // 38
	0x00, // 39 
	0x00, // 3A
	0x00, // 3B
	0x00, // 3C
	0x00, // 3D
	0x00, // 3E
	0x00, // 3F
	0x00, // 40
	0x00, // 41
	0x00, // 42
	0x00, // 43
	0x00, // 44
	0x00, // 45
	0x00, // 46
	0x00, // 47
	0x00, // 48
	0x00, // 49
	0x00, // 4A - 'KP /'
	0x00, // 4B
	0x00, // 4C
	0x00, // 4D
	0x00, // 4E
	0x00, // 4F
	0x00, // 50
	0x00, // 51
	0x00, // 52
	0x00, // 53
	0x00, // 54
	0x00, // 55
	0x00, // 56
	0x00, // 57
	0x00, // 58
	0x00, // 59
	0x00, // 5A - 'KP ENTER'
	0x00, // 5B
	0x00, // 5C
	0x00, // 5D
	0x00, // 5E
	0x00, // 5F
	0x00, // 60
	0x00, // 61
	0x00, // 62
	0x00, // 63
	0x00, // 64
	0x00, // 65
	0x00, // 66
	0x00, // 67
	0x00, // 68
	0x00, // 69 - 'END'
	0x00, // 6A
	0x00, // 6B - 'LEFT ARROW'
	0x00, // 6C - 'HOME'
	0x00, // 6D
	0x00, // 6E
	0x00, // 6F
	0x00, // 70 - 'INSERT'
	0x00, // 71 - 'DELETE'
	0x00, // 72 - 'DOWN ARROW'
	0x00, // 73
	0x00, // 74 - 'RIGHT ARROW'
	0x00, // 75 - 'UP ARROW'
	0x00, // 76
	0x00, // 77
	0x00, // 78
	0x00, // 79
	0x00, // 7A - 'PAD DOWN'
	0x00, // 7B
	0x00, // 7C
	0x00, // 7D - 'PAG UP'
	0x00, // 7E
	0x00, // 7F
	0x00, // 80
	0x00, // 81
	0x00, // 82
	0x00, // 83
	0x00, // 84
	0x00, // 85
	0x00, // 86
	0x00, // 87
	0x00, // 88
	0x00, // 89
	0x00, // 8A
	0x00, // 8B
	0x00, // 8C
	0x00, // 8D
	0x00, // 8E
	0x00, // 8F
	0x00, // 90
	0x00, // 91
	0x00, // 92
	0x00, // 93
	0x00, // 94
	0x00, // 95
	0x00, // 96
	0x00, // 97
	0x00, // 98
	0x00, // 99
	0x00, // 9A
	0x00, // 9B
	0x00, // 9C
	0x00, // 9D
	0x00, // 9E
	0x00, // 9F
	0x00, // A0
	0x00, // A1
	0x00, // A2
	0x00, // A3
	0x00, // A4
	0x00, // A5
	0x00, // A6
	0x00, // A7
	0x00, // A8
	0x00, // A9
	0x00, // AA
	0x00, // AB
	0x00, // AC
	0x00, // AD
	0x00, // AE
	0x00, // AF
	0x00, // B0
	0x00, // B1
	0x00, // B2
	0x00, // B3
	0x00, // B4
	0x00, // B5
	0x00, // B6
	0x00, // B7
	0x00, // B8
	0x00, // B9
	0x00, // BA
	0x00, // BB
	0x00, // BC
	0x00, // BD
	0x00, // BE
	0x00, // BF
	0x00, // C0
	0x00, // C1
	0x00, // C2
	0x00, // C3
	0x00, // C4
	0x00, // C5
	0x00, // C6
	0x00, // C7
	0x00, // C8
	0x00, // C9
	0x00, // CA
	0x00, // CB
	0x00, // CC
	0x00, // CD
	0x00, // CE
	0x00, // CF
	0x00, // D0
	0x00, // D1
	0x00, // D2
	0x00, // D3
	0x00, // D4
	0x00, // D5
	0x00, // D6
	0x00, // D7
	0x00, // D8
	0x00, // D9
	0x00, // DA
	0x00, // DB
	0x00, // DC
	0x00, // DD
	0x00, // DE
	0x00, // DF
	0x00, // E0
	0x00, // E1
	0x00, // E2
	0x00, // E3
	0x00, // E4
	0x00, // E5
	0x00, // E6
	0x00, // E7
	0x00, // E8
	0x00, // E9
	0x00, // EA
	0x00, // EB
	0x00, // EC
	0x00, // ED
	0x00, // EE
	0x00, // EF
	0x00, // F0
	0x00, // F1
	0x00, // F2
	0x00, // F3
	0x00, // F4
	0x00, // F5
	0x00, // F6
	0x00, // F7
	0x00, // F8
	0x00, // F9
	0x00, // FA
	0x00, // FB
	0x00, // FC
	0x00, // FD
	0x00, // FE
	0x00  // FF
};

void ps2k_callback(uint8_t *code, uint8_t count) {
	uint8_t amiga_scancode = 0;

	if (count == 0) { // Normal key pressed
		amiga_scancode = pgm_read_byte(&ps2_normal_convtable[code[0]]);
	} else if (count == 1 && code[0] == PS2_SCANCODE_RELEASE) { // Normal key depressed
		amiga_scancode = pgm_read_byte(&ps2_normal_convtable[code[1]]);		
	} else if (count == 1 && code[0] == PS2_SCANCODE_EXTENDED) { // Extended key pressed
		amiga_scancode = pgm_read_byte(&ps2_extended_convtable[code[1]]);		
	} else if (count == 2) { // Extended key depressed
		amiga_scancode = pgm_read_byte(&ps2_extended_convtable[code[2]]);				
	}

	printf("PS2   %.2X %.2X %.2X\n", code[0], code[1], code[2]);
	printf("AMIGA %.2X\n", amiga_scancode);
}
