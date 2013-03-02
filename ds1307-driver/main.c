 
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

	uint8_t readData[8];
	uint8_t writeData[7];

	writeData[0] = 0x00;
	writeData[1] = 0x33;
	writeData[2] = 0x00;
	writeData[3] = 0x7;
	writeData[4] = 0x03;
	writeData[5] = 0x03;
	writeData[6] = 0x13;

	I2C_Config *masterConfig = I2C_buildDefaultConfig();
	I2C_Device dsDevice;
	dsDevice.id = 0xD;
	dsDevice.address = 0;
	I2C_masterBegin(masterConfig);

	//I2C_masterWriteRegisterByte(&dsDevice, 0x00, writeData, 7);
	I2C_masterReadRegisterByte(&dsDevice, 0x00, readData, 8);

	while (1) {
		_delay_ms(3000);
		fprintf(stdout, "%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X\n", readData[0], readData[1], readData[2], readData[3], readData[4], readData[5], readData[6], readData[7]);
		I2C_masterReadRegisterByte(&dsDevice, 0x00, readData, 8);
	}

    return 0;
}
