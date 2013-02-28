 
#ifndef F_CPU
#define F_CPU 8000000UL // MCU Frequency set to 8Mhz
#endif

#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "main.h"
#include "uart.h"

int main(void) {
	// Initialize serial port for output
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	while (1) {
			_delay_ms(15);
	}

    return 0;
}
