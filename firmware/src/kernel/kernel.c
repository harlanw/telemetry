/**
 * @file kernel.c
 * @author Harlan J. Waldrop <waldroha@oregonstate.edu>
 * @date 2018-01-21
 * @brief Kernel processes and initialization routines
 */
#include "attributes.h"
#include "kernel/version.h"
#include "kernel/sch/tasks.h"
#include "kernel/serial/uart.h"
#include "kernel/sys/time.h"

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

time_t ktime = { 0 };	/**< Kernel timekeeping structure */

static void
ktime_tick(void);

ISR(TIMER1_OVF_vect)
{
	ktime_tick();
	ktask_schedule();
}

static void ATTR_CTOR
kinit()
{
	uart_init();

	stdout = &uart_out;
	stdin = &uart_in;

	// TODO: Add kernel logging system
	printf("INIT'D TELEMETRY KERNEL VER. %d.%d.%d (%d)\n",
		KERNEL_VERSION_MAJOR,
		KERNEL_VERSION_MINOR,
		KERNEL_VERSION_PATCH,
		KERNEL_VERSION_MAGIC);

	ktask_init();
}

static void ATTR_DTOR
kend(void)
{
	while (1);
}

void
kfinalize(void)
{
	TCCR1B |= _BV(CS00);
	TCNT1L = 0xFF & 999;
	TCNT1H = 999 >> 8;
	TIMSK1 |= _BV(TOIE1);

	sei();
}

void
ktime_tick(void)
{
	++ktime.ms;

	if (ktime.ms == 60000)
	{
		++ktime.m;
		ktime.ms = 0;
	}
}
