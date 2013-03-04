#ifndef _ANALOG_SENSORS_HEADER_
#define _ANALOG_SENSORS_HEADER_

#include <stdint.h>

void setupADC(uint8_t mux);
uint16_t readADC(void);

double adcReadToTemp(uint16_t readVal, double refVoltage, double seriesResistorVal, double thermistorNominal, double nominalTemp, uint16_t bCoeff);

#endif /* _ANALOG_SENSORS_HEADER_ */
