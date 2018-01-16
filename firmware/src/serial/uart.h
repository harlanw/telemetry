#ifndef UART_H
#define UART_H

#include <stdio.h>

void uart_init(void);

void uart_putchar(char c, FILE *stream);
char uart_getchar(FILE *stream);

extern FILE uart_out;
extern FILE uart_in;

#endif
