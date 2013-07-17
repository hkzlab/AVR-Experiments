#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "main.h"

#include "cdtv_commands.h"

#define IR_SHORT_PULSE 400 // uS
#define IR_LONG_PULSE 1200 // uS

#define IR_BEGIN_PAUSE 4500 // uS
#define IR_BEGIN_PULSE 9 // mS

void ir_begin(void);
void ir_pulse(void);
void ir_pause(uint8_t short_pause);
void setup_PWM(void);
void ir_send_command(uint8_t idx);

int main(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	printf("INIT!\n");

	DDRB = 0x00; // Disable DDRB
	setup_PWM();
   
	// Enable interrupts
	sei(); 

	while(1) {
		_delay_ms(5000);
		ir_send_command(13);
	}

	return 0;
}

void setup_PWM(void) {
 	// Turn off PWM while we set it up
	TCCR1B = 0;
	TCCR1A = 0;
	
	// Setup the timer MODE 8
   	TCCR1B |= (1 << WGM13);
   	TCCR1B |= (1 << CS10); //clock select prescaling /1 (which is 0.0625uS to increment the counter at 16Mhz)
   
	// Clear on compare match
	TCCR1A |= (1 << COM1A1);

	// Set ICR1 so the output toggles at 40KHz
	ICR1 = 200;
  
	// Set duty cycle to 25%
	OCR1A = (ICR1*25)/100;
}

void ir_pulse(void) {
	DDRB = 0xFF;
	_delay_us(IR_SHORT_PULSE);
	DDRB = 0x00;
}

void ir_pause(uint8_t long_pause) {
	if (long_pause)
		_delay_us(IR_LONG_PULSE);
	else
		_delay_us(IR_SHORT_PULSE);
}


void ir_begin(void) {
	DDRB = 0xFF;
	_delay_ms(IR_BEGIN_PULSE);
	DDRB = 0x00;
	_delay_us(IR_BEGIN_PAUSE);
}


void ir_send_command(uint8_t idx) {
	uint16_t command = pgm_read_word(&cdtv_ir_table[idx]);
	uint8_t counter;

	// Begin...
	ir_begin();

	for (counter = 0; counter < 12; counter++) {
		ir_pulse();
		ir_pause((command << counter) & 0x8000);
	}

	// Resend the command again, inverting the bits
	command = ~command;
	for (counter = 0; counter < 12; counter++) {
		ir_pulse();
		ir_pause((command << counter) & 0x8000);
	}

	// End pulse
	ir_pulse();
}
