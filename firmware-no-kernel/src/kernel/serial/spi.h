#ifndef SPI_H
#define SPI_H

#include <stdint.h>

void
spi_init(void);

uint8_t
spi_transmit(uint8_t send);

#endif
