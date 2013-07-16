#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "main.h"

void ir_pulse() {
	DDRB = 0xFF;
	_delay_us(400);
	DDRB = 0x00;
}

void ir_shortpause() {
	_delay_us(400);
}

void ir_longpause() {
	_delay_us(1200);
}

void send_test() {
	DDRB = 0xFF;
	_delay_ms(9);
	DDRB = 0x00;
	_delay_us(4500);

	// S
	ir_pulse();
	ir_shortpause();

	// S
	ir_pulse();
	ir_shortpause();

		// S
	ir_pulse();
	ir_shortpause();

		// S
	ir_pulse();
	ir_shortpause();

	// ----

	// S
	ir_pulse();
	ir_shortpause();


	// S
	ir_pulse();
	ir_shortpause();


	// S
	ir_pulse();
	ir_shortpause();


	// S
	ir_pulse();
	ir_shortpause();

	// ----

	// S
	ir_pulse();
	ir_shortpause();

	// S
	ir_pulse();
	ir_shortpause();

	// L
	ir_pulse();
	ir_longpause();

	// S
	ir_pulse();
	ir_shortpause();

	// INVERT

	// L
	ir_pulse();
	ir_longpause();

	// L
	ir_pulse();
	ir_longpause();

	// L
	ir_pulse();
	ir_longpause();

	// L
	ir_pulse();
	ir_longpause();

	// ----

	// L
	ir_pulse();
	ir_longpause();

	// L
	ir_pulse();
	ir_longpause();

	// L
	ir_pulse();
	ir_longpause();

	// L
	ir_pulse();
	ir_longpause();

	// ----

	// L
	ir_pulse();
	ir_longpause();

	// L
	ir_pulse();
	ir_longpause();

	// S
	ir_pulse();
	ir_shortpause();

	// L
	ir_pulse();
	ir_longpause();


	// END
	ir_pulse();

}

int main(void) {
	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	printf("INIT!\n");

	DDRB = 0xFF;

   // Turn off PWM while we set it up
   TCCR1B = 0;
   TCCR1A = 0;

   // 0.0625uS for incrementing counter
   // We need to go to 40Khz, which means 

   // Setup the timer MODE 8
   TCCR1B |= (1 << WGM13);
   //TCCR1A |= (1 << WGM10);
   TCCR1B |= (1 << CS10); //clock select prescaling /1
   // Setup the Compare Output to be set to toggle mode. See pg.108 in datasheet
   TCCR1A |= (1 << COM1A1);
   // Set ICR1 so the output toggles at 40KHz
   ICR1 = 200;
   // Set the Comparator so we're at 50% duty cycle
  // OCR1A = ICR1;
   
	OCR1A = (ICR1*25)/100;
   
	sei(); 

   uint8_t test = 0;
   while(1) {
	 //  test++;
	 //  _delay_ms(100);
	//	OCR1A = (ICR1*test)/100;

	//	test = test >= 100 ? 0 : test++;
   	send_test();
	   _delay_ms(2000);
   }

	return 0;
}

