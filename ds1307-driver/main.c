 
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

int main(void) {
	// Initialize serial port for output
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

	I2C_Config *masterConfig = I2C_buildDefaultConfig();
	I2C_Device dsDevice;
	dsDevice.id = 0xD;
	dsDevice.address = 0;
	I2C_masterBegin(masterConfig);

	uint8_t regData = 0;
	I2C_masterReadRegisterByte(&dsDevice, 0x01, &regData);

	while (1) {
			_delay_ms(3000);
			fprintf(stdout, "Loop! 0x%.2X\n", regData);
			I2C_masterReadRegisterByte(&dsDevice, 0x01, &regData);
	}

    return 0;
}
