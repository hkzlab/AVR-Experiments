#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include <stdio.h>
#include <stdlib.h>

#include <util/delay.h>

#include "main.h"
#include "uart.h"

#include "shifter_74595.h"
#include "hd44780-74595-interface.h"
#include "hd44780-highlevel.h"

#include "owilib.h"
#include "ds18b20.h"

#define DEBOUNCE_TIME 25

#define MINIMUM_TEMP_THRESH 0
#define MAXIMUM_TEMP_THRESH 99

static hd44780_driver *lcdDriver = NULL;
static owi_conn *dsconn;
static uint8_t *sensor_addrs, sensor_count;

static volatile float current_temperature = 0.0f;
static volatile float temperature_thresh = 30.0f;

static volatile uint8_t need_update = 0;

void sys_setup(void);
void timer_init(void);
void get_temp(void);
void update_lcd(void);

int main(void) {
	sys_setup();

	// First temp conversion
	ds18b20_startTempConversion(dsconn, NULL);
	_delay_ms(3000);

	hd44780_hl_clear(lcdDriver);

	while(1) {
		if (!(PIND & (1 << PD2))) {
			temperature_thresh += 0.5f;
			if (temperature_thresh > MAXIMUM_TEMP_THRESH) temperature_thresh = MAXIMUM_TEMP_THRESH;
			
			update_lcd();
			_delay_ms(DEBOUNCE_TIME);
		} else if (!(PIND & (1 << PD3))) {
			temperature_thresh -= 0.5f;
			if (temperature_thresh < MINIMUM_TEMP_THRESH) temperature_thresh = MINIMUM_TEMP_THRESH;
			
			update_lcd();
			_delay_ms(DEBOUNCE_TIME);
		}

		if (need_update) {
			get_temp();
			ds18b20_startTempConversion(dsconn, NULL);
			update_lcd();
		}

		if (current_temperature > temperature_thresh) {
			PORTB &= !(1 << PB1);
		} else if (current_temperature <= temperature_thresh) {
			PORTB |= (1 << PB1);
		}
	}

	return 0;
}

void timer_init(void) {
    // set up timer with prescaler 256 
    TCCR0B |= (1 << CS01) | (1 << CS00);
		 
    // initialize counter
    TCNT0 = 0;

	TIMSK0 |= (1 << TOIE0); // Enable timer 0 overflow interrupt
}

void update_lcd(void) {
	cli();

	char strbuf[20];

	snprintf(strbuf, 16, "% 07.2f  <%04.1f> ", current_temperature, temperature_thresh);
	hd44780_hl_printText(lcdDriver, 0, 0, strbuf);

	sei();
}

void get_temp(void) {
	char strbuf[20];
	
	int8_t integ;
	uint16_t decim;

	float cur_temperature = 0.0f;
	float avg_temperature = 0.0f;

	for (uint8_t idx = 0; idx < sensor_count; idx++) {
		ds18b20_getTemp(dsconn, sensor_addrs + (8 * idx), &integ, &decim);
		snprintf(strbuf, 14, "%d.%u", integ, decim);
		sscanf(strbuf, "%f", &cur_temperature);

		if (avg_temperature == 0.0f) {
			avg_temperature = cur_temperature;	
		} else {
			avg_temperature += cur_temperature;
			avg_temperature /= 2;
		}
	}

	current_temperature = avg_temperature;
}

void sys_setup(void) {
	char strbuf[20];

	cli();

	// Initialize serial port for output
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	// Led connections
	DDRB |= (1 << PB1);
	PORTB &= !(1 << PB1);

	// Shifter connections
	DDRC |= 0x0F; // First 4 pins of Port C set as output
	PORTC &= 0xF0;

	// Sensor connections
	dsconn = (owi_conn *)malloc(sizeof(owi_conn));
	dsconn->port = &PORTD;
	dsconn->pin = &PIND;
	dsconn->ddr = &DDRD;
	dsconn->pinNum = 7;

	// Put PD2 and PD3 as input, with pull-up resistors, for buttons
	DDRD &= !((1 << PD2) | (1 << PD3));
	PORTD |= ((1 << PD2) | (1 << PD3));

	// Init LCD
	shifter_74595_conn *shfConn = shf74595_createConnection(&PORTC, 3, &PORTC, 2, &PORTC, 1, &PORTC, 0);
	shf74595_initConnection(shfConn);

	hd44780_74595_connection *conn_struct = hd44780_74595_createConnection(shfConn, 0, 5, 4);
	lcdDriver = hd44780_hl_createDriver(PVC160101Q_16x1, conn_struct, (uint8_t ( *)(void *))hd44780_74595_initLCD4Bit, (void ( *)(void *, uint16_t))hd44780_74595_sendCommand);
	hd44780_hl_init(lcdDriver, 0, 0);

	// Print first line
	hd44780_hl_clear(lcdDriver);

	/*
	// Enable interrupts
	EIMSK |= 1 << INT1;  //Enable INT1
	EICRA |=  (1 << ISC11); //Trigger on falling edge of INT1
	EICRA &= !(1 << ISC10);
	
	EIMSK |= 1 << INT0;  //Enable INT0
	EICRA |=  (1 << ISC01); //Trigger on falling edge of INT0
	EICRA &= !(1 << ISC00);
	*/

	owi_reset(dsconn);
	owi_searchROM(dsconn, NULL, &sensor_count, 0);
	
	snprintf(strbuf, 16, "SENS: \n%u\n", sensor_count);
	hd44780_hl_printText(lcdDriver, 0, 0, strbuf);

	if (sensor_count) {
		sensor_addrs = (uint8_t *)malloc(8 * sensor_count);
		owi_searchROM(dsconn, sensor_addrs, &sensor_count, 0);

		ds18b20_cfg dscfg;
		dscfg.thrmcfg = DS_THRM_12BIT;
		dscfg.lT = 0;
		dscfg.hT = 0;
		ds18b20_setCFG(dsconn, NULL, &dscfg);
	}

	timer_init();

	sei();

	fprintf(stdout, "Starting up.\n");
}

// INTERRUPTS

ISR (TIMER0_OVF_vect) {
	need_update = 1;
}

ISR(INT0_vect) {
/*
	temperature_thresh += 0.5f;
	if (temperature_thresh > MAXIMUM_TEMP_THRESH) temperature_thresh = MAXIMUM_TEMP_THRESH;
*/
}

ISR(INT1_vect) {
/*
	temperature_thresh -= 0.5f;
	if (temperature_thresh < MINIMUM_TEMP_THRESH) temperature_thresh = MINIMUM_TEMP_THRESH;
*/
}

