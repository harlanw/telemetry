/**
 * @file panic.c
 * @author Harlan J. Waldrop <waldroha@oregonstate.edu>
 * @date 2018-01-21
 *
 * @brief Kernel panic routine
 *
 * A kernel panic is an event from which the kernel cannot easily recover. These errors are
 * typically, but not always, fatal.
 */
#include "system.h"

#include <stdio.h>
#include <avr/interrupt.h>

void
panic(char *rsn, const char *file, int line)
{
	cli();

	puts("kernel_panic:");
	printf(">> reason: %s\n", rsn);
	printf(">> file: %s\n", file);
	printf(">> line: %d\n", line);

	SLEEP();
}
