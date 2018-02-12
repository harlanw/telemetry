/**
 * @file kernel.c
 * @author Harlan J. Waldrop <waldroha@oregonstate.edu>
 * @date 2018-01-21
 * @brief Kernel processes and initialization routines
 */
#include "attributes.h"
#include "kernel/version.h"
#include "kernel/serial/spi.h"
#include "kernel/serial/uart.h"
#include "kernel/sys/time.h"

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

static time_t ktime = { 0 };	/**< Kernel timekeeping structure */

static void
ktime_tick(void);

ISR(TIMER1_COMPA_vect)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		ktime_tick();
	}
}

static void ATTR_CTOR
kinit()
{
	uart_init();

	stdout = &uart_out;
	stdin = &uart_in;

	// TODO: Add kernel logging system
	printf("INIT'D TELEMETRY KERNEL VER. %d.%d.%d (%d)\nBUILD: %s\n",
		KERNEL_VERSION_MAJOR,
		KERNEL_VERSION_MINOR,
		KERNEL_VERSION_PATCH,
		KERNEL_VERSION_MAGIC,
		__TIMESTAMP__);

	DDRA = DDRB = 0;
	PORTA = PORTB = 0;

	spi_init();
}

static void ATTR_DTOR
kend(void)
{
	while (1);
}

void
kfinalize(void)
{
	TCCR1B |= _BV(WGM12) | _BV(CS00);
	OCR1A = 999;
	TIMSK1 |= _BV(OCIE1A);

	sei();
}

static void
ktime_tick(void)
{
	++ktime.ms;

	if (ktime.ms == 1000)
	{
		++ktime.s;
		ktime.ms = 0;

		if (ktime.s == 60)
		{
			++ktime.m;
			ktime.s = 0;
		}
	}
}

time_t
now(void)
{
	return ktime;
}
