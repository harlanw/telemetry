#include "uart.h"

#include <avr/io.h>

#ifndef F_CPU
	#define FCPU 1000000UL
#endif

FILE uart_out = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_in  = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void
uart_putchar(char c, FILE *stream)
{
	while (!(UCSR0A & (1 << UDRE0)));

	UDR0 = c;
}

char
uart_getchar(FILE *stream)
{
	while (!(UCSR0A & (1 << UDRE0)));

	return UDR0;
}

void
uart_init(void)
{
	UBRR0L = 12;
	UBRR0H = 12 >> 8;

#ifdef UART_USE_2X
	UCSR0A |= _BV(U2X0);
#else
	UCSR0A &= ~(_BV(U2X0));
#endif
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}
