#include <avr/io.h>
#include <stdio.h>

#ifndef BAUD
#define BAUD 19200
#endif 

#include <util/setbaud.h>

/* http://www.cs.mun.ca/~rod/Winter2007/4723/notes/serial/serial.html */

#if defined(__AVR_AT90Tiny2313__) | defined(__AVR_ATtiny2313__) | defined(__AVR_ATtiny4313__)
#define UBRR0H UBRRH
#define UBRR0L UBRRL

#define UCSR0A UCSRA
#define UCSR0C UCSRC
#define UCSR0B UCSRB

#define RXEN0 RXEN
#define TXEN0 TXEN

#define UCSZ00 UCSZ0
#define UCSZ01 UCSZ1

#define UDR0 UDR
#define UDRE0 UDRE

#define RXC0 RXC
#endif

void uart_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8-bit data */ 
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);   /* Enable RX and TX */    
}

int uart_putchar(char c, FILE *stream) {
    if (c == '\n') {
        uart_putchar('\r', stream);
    }
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;

    return 0;
}

int uart_getchar(FILE *stream) {
    loop_until_bit_is_set(UCSR0A, RXC0);

    return UDR0;
}
