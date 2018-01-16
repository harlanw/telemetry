#include "system.h"

#include <stdio.h>

void
panic(char *rsn, const char *file, int line)
{
	// TODO: disable IRQ, SLEEP
	puts("KERNEL PANIC:");
	printf("REASON: %s\n", rsn);
	printf("FILE: %s\n", file);
	printf("LINE: %d\n", line);
}
