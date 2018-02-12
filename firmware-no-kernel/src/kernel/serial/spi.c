#include "kernel/serial/spi.h"

#include <avr/io.h>

void
spi_init(void)
{
	DDRB = _BV(7) | _BV(5);
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0) | (1 << SPR1) | (1 << SPR0);
}

uint8_t
spi_transmit(uint8_t send)
{
	SPDR = send;

	while (!(SPSR & (1 << SPIF)));

	return (SPDR);
}
