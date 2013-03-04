#include "analog_sensors.h"

#include <avr/io.h>
#include <math.h>

// Setup ADC
// See here: http://www.avrbeginners.net/
// http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=56429
// http://www.protostack.com/blog/2011/02/analogue-to-digital-conversion-on-an-atmega168/
// http://www.pjrc.com/teensy/adc.html

void setupADC(uint8_t mux) {
	ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADMUX = (1 << REFS0) | mux;	
	ADCSRA |= (1 << ADEN);
}

uint16_t readADC(void) {
	uint8_t low, high;

	ADCSRA |= (1 << ADSC); // Start ADC read

	while (ADCSRA & (1 << ADSC)); // Wait for conversion to finish

	low = ADCL;
	high = ADCH;

	return (high << 8) | low;
}

double adcReadToTemp(uint16_t readVal, double refVoltage, double seriesResistorVal, double thermistorNominal, double nominalTemp, uint16_t bCoeff) {
	double voltage = (readVal / 1023.0f) * refVoltage;
	double res = ((seriesResistorVal * refVoltage) / voltage) - seriesResistorVal;

	double temp = log(res / thermistorNominal) / bCoeff;
	temp += 1.0f / (nominalTemp + 273.15f);
	temp = 1.0 / temp;
	temp -= 273.15f;	

	return temp;
}
