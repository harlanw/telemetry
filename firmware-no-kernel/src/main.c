/**
 * @file main.c
 * @author Harlan J. Waldrop <waldroha@oregonstate.edu>
 * @date 2018-01-21
 *
 * @brief Post-kernel-initialization routines for kernel
 *
 * This file is responsible for adding and initializing any extra kernel tasks, as well as any other
 * routines required for system operation.
 *
 * @see ktask_add()
 * @see kinit()
 * @see kend()
 */
#include "kernel/sys/system.h"
#include "kernel/version.h"
#include "kernel/dev/modem.h"
#include "kernel/driver/xbs2.h"
#include "kernel/serial/spi.h"
#include "kernel/serial/uart.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#define XB_CS		3

#define XB_ATTN		0
#define XB_SLEEP	1
#define XB_RST		2
#define XB_DIN		3
#define XB_DOUT		4

uint8_t seek_frame = 0;
modem_t modem;

const char *str = "{humid:0.18,temp:65.3,bright:false,nmea:\"$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\",\"imu\": {\"error\": \"disconnected\"}}\n\n";

void
user_process(void)
{
	DDRA |= _BV(XB_CS) | _BV(0);
	PORTA = _BV(0);
	DDRB |= _BV(4) | _BV(3) | _BV(2) | _BV(1);
	PORTB |= _BV(XB_ATTN) | _BV(XB_RST);

	modem_create(&modem);
	modem.connect(NULL, 0);

	while (modem.update() != -1)
	{
		if (modem.connection != MODEM_JOINED)
		{
			printf("<USER> %s", str);
			PANIC("FAILED TO CONNECT TO NETWORK");
		}
		else
		{
			modem.write(str, sizeof(str));
		}

		_delay_ms(100);
	}
/*
	int ro = 1;
	while (1)
	{
		xbs2_poll();

		if ((PINB & _BV(XB_ATTN)))
		{
			/*
			if (ro)
			{
				uint8_t pkt[2] = {'H', 'V'};
				xbs2_at(pkt, NULL, 0);
				ro = 0;
				seek_frame = 1;
			}
		}
	}
	*/
}

/**
 * @brief System invocation routines
 *
 * @note On return immediately yields to kernel's end process, @kend()
 */
int
main(void)
{
	// TODO: Add kernel logging system
	kfinalize();
	user_process();

	return 0;
}
