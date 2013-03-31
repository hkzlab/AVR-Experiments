#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#include "main.h"
#include "uart.h"

#include "shifter_74595.h"
#include "hd44780-74595-interface.h"
#include "hd44780-highlevel.h"

#include "twilib.h"
#include "ds1307.h"

#define  SRAM_CHKVAL 0xDEAD

static hd44780_driver *lcdDriver = NULL;
static volatile int8_t pressedKey = -1;
static volatile int8_t clock_sec = 0;

static uint8_t lcd_graphics[4][8] = {{
		0b00001110,
		0b00011011,
		0b00011111,
		0b00001110,
		0b00000110,
		0b00001110,
		0b00000110,
		0b00001110
	},
	{
		0b00001110,
		0b00010001,
		0b00010001,
		0b00010001,
		0b00001110,
		0b00001110,
		0b00001110,
		0b00000100
	},
	{
		0b00011100,
		0b00011110,
		0b00011110,
		0b00000110,
		0b00000110,
		0b00011110,
		0b00011110,
		0b00011100
	},
	{
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00001011,
		0b00011111,
		0b00011111,
		0b00001000
	}
};

void sys_setup(void);
void clock_setup(uint8_t backEnabled);
int clock_checkAndSet(int8_t *data);

int main(void) {
	char timeStrBuffer[21];

	sys_setup();

	DS1307_ToD time;
	DS1307_readToD(&time);
	sprintf(timeStrBuffer, "%.2u/%.2u/%.4u     %.2u:%.2u", time.dayOfMonth, time.month, time.year, time.hours, time.minutes);
	hd44780_hl_printText(lcdDriver, 0, 0, timeStrBuffer);

	uint8_t clkCounter = 0;
	while (1) {
		if (clock_sec) {
			clock_sec = 0;
			clkCounter++;
		}

		if (!(clkCounter % 30)) {
			clkCounter = 0;
		
			DS1307_readToD(&time);
			sprintf(timeStrBuffer, "%.2u/%.2u/%.4u     %.2u:%.2u", time.dayOfMonth, time.month, time.year, time.hours, time.minutes);
			hd44780_hl_printText(lcdDriver, 0, 0, timeStrBuffer);
		}

		switch (pressedKey) {
			case 3:
				clock_setup(1);

				DS1307_readToD(&time);
				sprintf(timeStrBuffer, "%.2u/%.2u/%.4u     %.2u:%.2u", time.dayOfMonth, time.month, time.year, time.hours, time.minutes);
				hd44780_hl_printText(lcdDriver, 0, 0, timeStrBuffer);

				break;
			default:
				break;
		}

		if (pressedKey >= 0) pressedKey = -1;
	}

	return 0;
}

void sys_setup(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	// Shifter connections
	DDRC |= 0x0F; // First 4 pins of Port C set as output
	PORTC &= 0xF0;

	// Key encoder connections
	DDRB &= 0xF0; // First 4 pins of Port B set as Input
	PORTB &= 0xF0;
	DDRD &= 0x7F; // Last pin of Port D set as Input (will be used as interrupt)
	PORTD &= 0x7F;

	// Enable interrupts
	PCICR |= 1 << PCIE2; // Enable PCINT2 (23..16)
	PCMSK2 |= 1 << PCINT23;
	sei();

	// Init LCD
	shifter_74595_conn *shfConn = shf74595_createConnection(&PORTC, 3, &PORTC, 2, &PORTC, 1, &PORTC, 0);
	shf74595_initConnection(shfConn);

	hd44780_74595_connection *conn_struct = hd44780_74595_createConnection(shfConn, 0, 5, 4);
	lcdDriver = hd44780_hl_createDriver(TMBC20464BSP_20x4, conn_struct, (uint8_t ( *)(void *))hd44780_74595_initLCD4Bit, (void ( *)(void *, uint16_t))hd44780_74595_sendCommand);
	hd44780_hl_init(lcdDriver, 0, 0);

	hd44780_hl_setCustomFont(lcdDriver, 4, lcd_graphics[0]);
	hd44780_hl_setCustomFont(lcdDriver, 5, lcd_graphics[1]);
	hd44780_hl_setCustomFont(lcdDriver, 6, lcd_graphics[2]);
	hd44780_hl_setCustomFont(lcdDriver, 7, lcd_graphics[3]);


	// Init TWI
	I2C_Config *masterConfig = I2C_buildDefaultConfig();
	I2C_masterBegin(masterConfig);

	// Print first line
	hd44780_hl_clear(lcdDriver);
	hd44780_hl_printText(lcdDriver, 0, 0, "Base Sensori\nAttendere...");
	_delay_ms(2000);

	// Check the clock
	DDRD &= 0xBF; // Set pin 6 of Port D as input. Will be used as interrupt
	PORTD &= 0xBF;

	fprintf(stdout, "Checking DS1307 status...\n");
	uint16_t ds_chkval;
	DS1307_readSRAM((uint8_t *)&ds_chkval, 2);
	if (ds_chkval != SRAM_CHKVAL) { // The clock lost power!
		ds_chkval = SRAM_CHKVAL;

		clock_setup(0);

		DS1307_writeSRAM((uint8_t *)&ds_chkval, 2); // Mark clock SRAM
	} else { // Clock is fine
		fprintf(stdout, "Clock check passed!!!\n");
	}
	DS1307_setSQW(1, 0, DS1307_SQW_1Hz);
	PCMSK2 |= 1 << PCINT22;


	hd44780_hl_clear(lcdDriver);
}

int clock_checkAndSet(int8_t *data) {
	DS1307_ToD time;

	for (uint8_t counter = 0; counter < 10; counter++) {
		if (data[counter] < 0) return 0;
	}

	time.seconds = 0;
	time.minutes = data[8] * 10 + data[9];
	if (time.minutes > 59) return 0;
	time.hours = data[6] * 10 + data[7];
	if (time.hours > 23) return 0;

	time.dayOfMonth = data[0] * 10 + data[1];
	if (time.dayOfMonth > 31) return 0;
	time.month = data[2] * 10 + data[3];
	if (time.month > 12) return 0;
	time.year = data[4] * 10 + data[5] + 2000;

	time.dayOfWeek = dayOfWeek(time.dayOfMonth, time.month, time.year);

	DS1307_writeToD(&time);
	DS1307_setSQW(1, 0, DS1307_SQW_1Hz);	

	return 1;
}

void clock_setup(uint8_t backEnabled) {
	uint8_t key_table[] = {0x31, 0x32, 0x33, 0x04, 0x34, 0x35, 0x36, 0x05, 0x37, 0x38, 0x39, 0x06, 0x23, 0x30, 0x2a, 0x07};
	uint8_t skip_table[] = {0, 1, 3, 4, 8, 9, 15, 16, 18, 19};
	int8_t timeData[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	uint8_t cur_pos = 0;

	hd44780_hl_clear(lcdDriver);
	hd44780_hl_printText(lcdDriver, 0, 0, "Config. orario:\nDD/MM/20YY     hh:mm");

	if (backEnabled)
		hd44780_hl_printText(lcdDriver, 3, 0, "\x7F#    \x04\NO  \x07OK    *\x7E");
	else
		hd44780_hl_printText(lcdDriver, 3, 0, "\x7F#      \x07OK       *\x7E");		

	hd44780_hl_moveCursor(lcdDriver, 1, skip_table[cur_pos]);
	hd44780_hl_setCursor(lcdDriver, 1, 1);	

	uint8_t looping = 1;
	while (looping) {
		switch (pressedKey) {
			case -1:
				continue;
			case 3:
				if (backEnabled)
					looping = 0;
				break;
			case 0:
			case 1:
			case 2:
			case 4:
			case 5:
			case 6:
			case 8:
			case 9:
			case 10:
			case 13:
				hd44780_hl_printCharAtCurrentPosition(lcdDriver, key_table[pressedKey]);
				timeData[cur_pos] = key_table[pressedKey] - 0x30;

				cur_pos = (cur_pos + 1) % sizeof(skip_table);
				hd44780_hl_moveCursor(lcdDriver, 1, skip_table[cur_pos]);		
				break;
			case 12: // Left
				cur_pos = (((cur_pos - 1) < 0) ? (sizeof(skip_table) - 1) : (cur_pos - 1));
				hd44780_hl_moveCursor(lcdDriver, 1, skip_table[cur_pos]);				
				break;
			case 14: // Right
				cur_pos = (cur_pos + 1) % sizeof(skip_table);
				hd44780_hl_moveCursor(lcdDriver, 1, skip_table[cur_pos]);		
				break;
			case 15: // Enter
				if (clock_checkAndSet(timeData)) {
					looping = 0;
				} else {
					hd44780_hl_printText(lcdDriver, 2, 0, "    ** ERRORE **");
					hd44780_hl_moveCursor(lcdDriver, 1, skip_table[cur_pos]);							
				}
				break;
			default:
				break;
		}

		if (pressedKey >= 0) pressedKey = -1;
	}

	hd44780_hl_clear(lcdDriver);	
	hd44780_hl_setCursor(lcdDriver, 0, 0);		
}

// INTERRUPTS
ISR(PCINT2_vect) { // PCINT 23..16
	static uint8_t oldPinD = 0x00;

	// Check for key
	if ((oldPinD & 0x80) != (PIND & 0x80)) { // Key encoder interrupt
		if (PIND & 0x80)
			pressedKey = PINB & 0xF;
		else
			pressedKey = -1;
	}

	// Check for clock
	if ((oldPinD & 0x40) != (PIND & 0x40)) { // DS1307 interrupt
		if (PIND & 0x40)
			clock_sec = 1;
		else
			clock_sec = 0;
	}

	oldPinD = PIND;	
}
