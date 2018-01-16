#include "kernel.h"

#include "attributes.h"
#include "serial/uart.h"

#include <stdio.h>

void ATTR_CTOR
kernel_init()
{
	uart_init();

#ifdef DEBUG
	stdout = &uart_out;
	stdin = &uart_in;

	puts("INIT'D TELEMETRY KERNEL VER. " KERNEL_VERSION_STR);
#endif
}
