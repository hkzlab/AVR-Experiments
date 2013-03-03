 
#ifndef F_CPU
#define F_CPU 8000000UL // MCU Frequency set to 8Mhz
#endif

#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "main.h"
#include "uart.h"

#include "twilib.h"
#include "ds1307.h"

int main(void) {
	// Initialize serial port for output
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	DS1307_ToD time;

	I2C_Config *masterConfig = I2C_buildDefaultConfig();
	I2C_masterBegin(masterConfig);

	DS1307_readToD(&time);

	while (1) {
		DS1307_readToD(&time);
		_delay_ms(3000);

		fprintf(stdout, "%.2u-%.2u-%.4u %.2u:%.2u:%.2u\n", time.dayOfMonth, time.month, time.year, time.hours, time.minutes, time.seconds);
	}

    return 0;
}
