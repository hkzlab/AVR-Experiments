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

#define REPEAT_TRIGGER_TIME 937 // 60 / (1000 / (F_CPU / 1024))

void ir_begin(void);
void ir_pulse(void);
void ir_repeat(void);
void ir_pause(uint8_t short_pause);
void setup_PWM(void);
void ir_send_command(uint16_t command);

int main(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	printf("INIT!\n");

	DDRA = 0xFF; // Port A as output

	DDRB = 0x00; // Disable DDRB
	setup_PWM();
   
	// Enable interrupts
	sei(); 

	DDRE = 0x00; // Port E as input
	PORTE = 0xFF; // Set pullup resistor

	TCCR3B |= (1 << CS32) | (1 << CS30);

	uint8_t ctrl_status = 0xFF; // All released
	uint8_t old_ctrl_status  = ctrl_status;
	uint16_t ir_cmd = 0x0000;

	TCNT3 = 0;
	while(1) {
		old_ctrl_status = ctrl_status;
		ctrl_status = PINE;

		if ((old_ctrl_status != ctrl_status) && (ctrl_status != 0xFF)) {
			ir_cmd = 0x0000;

			if (~ctrl_status & 0x1) { // up
				ir_cmd |= pgm_read_word(&cdtv_ir_table[29]);
			} else if (~ctrl_status & 0x2) { // down
				ir_cmd |= pgm_read_word(&cdtv_ir_table[31]);
			} 
			
			if (~ctrl_status & 0x4) { // left 
				ir_cmd |= pgm_read_word(&cdtv_ir_table[32]);
			} else if (~ctrl_status & 0x8) { // right
				ir_cmd |= pgm_read_word(&cdtv_ir_table[30]);
			} 
			
			if (~ctrl_status & 0x40) { // A
				ir_cmd |= pgm_read_word(&cdtv_ir_table[27]);
			} 
			
			if (~ctrl_status & 0x80) { // B
				ir_cmd |= pgm_read_word(&cdtv_ir_table[28]);
			}
			
			ir_send_command(ir_cmd);

			TCNT3 = 0;
		}

		if (TCNT3 >= REPEAT_TRIGGER_TIME) {
			if (ctrl_status != 0xFF)
				ir_repeat();

			TCNT3 = 0;
		}
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
	if (long_pause) {
		_delay_us(IR_LONG_PULSE);
	} else {
		_delay_us(IR_SHORT_PULSE);
	}
}

void ir_repeat(void) {
	PORTA = 0x02;

	// Sends repeat code
	DDRB = 0xFF;
	_delay_ms(9);
	DDRB = 0x00;
	_delay_us(2100);
	ir_pulse();

	PORTA = 0x00;
}


void ir_begin(void) {
	DDRB = 0xFF;
	_delay_ms(IR_BEGIN_PULSE);
	DDRB = 0x00;
	_delay_us(IR_BEGIN_PAUSE);
}


void ir_send_command(uint16_t command) {
//	uint16_t command = pgm_read_word(&cdtv_ir_table[idx]);
	int8_t counter;

	PORTA = 0x01;

	// Begin...
	ir_begin();

	for (counter = 11; counter >= 0; counter--) {
		ir_pulse();
		ir_pause((command >> (counter + 4)) & 1);
	}

	command = ~command;

	// Resend the command again, inverting the bits
	for (counter = 11; counter >= 0; counter--) {
		ir_pulse();
		ir_pause((command >> (counter + 4)) & 1);
	}

	// End pulse
	ir_pulse();

	PORTA = 0x00;
}
