#include "kernel/driver/xbs2.h"

#include "kernel/sys/system.h"
#include "kernel/serial/spi.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>

#define XB_CS		3

#define XB_ATTN		0
#define XB_SLEEP	1
#define XB_RST		2
#define XB_DIN		3
#define XB_DOUT		4

#define XBS2_BROADCAST_MAX_HOPS 0x00

static modem_t *modem;

static uint8_t frames[3][30];
static uint8_t frame = 0;
static uint8_t frame_i= 0;
static uint8_t frame_length = 0;

static void
transmit(uint8_t byte)
{
	uint8_t res;

	res = spi_transmit(byte);

	if (res == 0x7E)
	{
		if (frame == 3)
		{
			frame = 0;
			frame_i = 0;
		}

		frames[frame][frame_i++] = res;
	}
	else if (frame_i)
	{
		if (frame_i == 20)
		{
			++frame;
			frame_i = 0;

			return;
		}
		else if (frame_i == 2)
		{
			frame_length = (frames[frame][1] << 8) + res + 1;
		}
		else if (frame_length)
		{
			--frame_length;

			if (!frame_length)
			{
				frames[frame][frame_i] = res;

				uint16_t size = (frames[frame][1] << 8) + frames[frame][2] + 4;
				uint8_t *f = frames[frame];
				printf("<MODEM> RX ");
				for (uint8_t i = 0; i < size; i++)
				{
					printf("%.2X ", f[i]);
				}
				printf("(%d)\n", size);

				++frame;
				frame_i = 0;

				return;
			}
		}

		frames[frame][frame_i++] = res;
	}
}

void
xbs2_init(modem_t *m)
{
	modem = m;
}

int8_t
xbs2_connect(char psk[], uint16_t size)
{
	uint8_t cmd[2], args[1];
	uint8_t fid;
	xbs2_frame_t f;

	if (size)
		return -1;

	// CONFIGURE AS END DEVICE
	cmd[0] = 'S';
	cmd[1] = 'M';
	args[0] = 1;
	xbs2_at(cmd, args, 1);

	// QUERY NETWORK STATUS
	cmd[0] = 'A';
	cmd[1] = 'I';
	fid = xbs2_at(cmd, NULL, 0);

	printf("<MODEM> SEARCHING...\n");
	_delay_ms(1000);
	for (uint8_t i = 0; i < 3; i++)
	{
		xbs2_update();
	}

	f = xbs2_get_frame_by_id(fid);
	if (f.type == -1 | f.size > 8 | f.pkt[8] == 0xFF)
	{
		printf("<MODEM> CONNECTION TIMEOUT\n");
		modem->connection = MODEM_FAILED; 
		return -1;
	}
	else
	{
		modem->connection = MODEM_JOINED;
	}

	return 0;
}

int8_t
xbs2_update(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (!(PINB & _BV(XB_ATTN)))
		{
			PORTA &= ~(_BV(XB_CS));
			while (!(PINB & _BV(XB_ATTN)))
			{
				transmit(0xFF);
			}
			PORTA |= _BV(XB_CS);
		}
	}

	return 0;
}

void
xbs2_query(modem_query_t q)
{
	uint8_t cmd[2];

	switch (q)
	{
		case MODEM_STATUS:
			cmd[0] = 'A';
			cmd[1] = 'I';
			xbs2_at(cmd, NULL, 0);
			break;
	}
}

xbs2_frame_t
xbs2_get_frame_by_id(uint8_t id)
{
	xbs2_frame_t result = { 0 };

	result.type = -1;

	for (uint8_t i = 0; i < 3; i++)
	{
		if (frames[i][4] == id)
		{
			uint16_t size = (frames[i][1] << 8) + frames[i][2] + 4;

			result.size = size;

			for (uint8_t j = 0; j < result.size; j++)
			{
				result.pkt[j] = frames[i][j];
			}

			result.type = 0;
		}
	}

	return result;
}

void
xbs2_write(char msg[], uint16_t size)
{
	const uint8_t broadcast[] =
	{
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0xFF
	};

	const uint8_t dest_unk[] =
	{
		0xFF, 0xFE
	};

	uint8_t *pkt, fid;
	uint16_t length = 14 + size;
	xbs2_frame_t f;

	pkt = malloc(sizeof(length + 4));
	pkt[0] = 0x7E;
	pkt[1] = (length >> 8);
	pkt[2] = (length & 0xFF);
	pkt[3] = 0x10;
	pkt[4] = 0x00;
	for (uint8_t i = 0; i < 8; i++)
		pkt[5 + i] = broadcast[i];
	for (uint8_t i = 0; i < 2; i++)
		pkt[13 + i] = dest_unk[i];
	pkt[15] = XBS2_BROADCAST_MAX_HOPS;
	pkt[16] = 0x00;
	for (uint8_t i = 0; i < size; i++)
		pkt[17 + i] = msg[i];

	fid = xbs2_emit(pkt, length + 4);
	_delay_ms(1);
	for (uint8_t i = 0; i < 3; i++)
		xbs2_update();
	f = xbs2_get_frame_by_id(fid);

	if (f.pkt[8] == 0x22)
	{
		puts("<MODEM> NOT JOINED TO A NETWORK");
		printf("\tDIED ON:%s\n", msg);
	}

	free(pkt);
/*
	uint16_t size = 14 + length;
	printf("<MODEM> TX 7E %0.2X %0.2X 10 03 ",
		(uint8_t) (size >> 8),
		(uint8_t) (size & 0xFF));
	printf("00 00 00 00 00 00 FF FF FF FE 00 00 ");
	for (uint16_t i = 0; i < length; i++)
	{
		printf("%.2X ", msg[i]);
	}
	printf("(%d)", size);
	printf("<MODEM> MSG %s", msg);
	*/
}

uint8_t
xbs2_emit(uint8_t pkt[], uint16_t length)
{
	static uint8_t id = 0;

	uint16_t size;
	uint8_t chksum;

	size = (pkt[1] << 8) + pkt[2];
	chksum = 0xFF;

	if (!id)
		++id;

	pkt[4] = id++;
	for (int i = 3; i < length - 1; i++)
		chksum -= pkt[i];

	pkt[length - 1] = chksum;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		PORTA &= ~(_BV(XB_CS));
		printf("<MODEM> TX ");
		for (int i = 0; i < length; i++)
		{
			printf("%.2X ", pkt[i]);
			transmit(pkt[i]);
		}
		puts("<<<");
		PORTA |= _BV(XB_CS);
	}

	return pkt[4];
}

uint8_t
xbs2_at(
	uint8_t cmd [static 2],
	uint8_t args[],
	uint8_t argc)
{
	uint8_t pkt[10] = { 0 };
	uint16_t length = 4 + argc;

	uint8_t fid = 0;

	pkt[0] = 0x7E;
	pkt[1] = (length >> 8);
	pkt[2] = (length & 0xFF);
	pkt[3] = XBS2_AT_CMD;
	//pkt[4] = 0;
	pkt[5] = cmd[0];
	pkt[6] = cmd[1];
	for (uint8_t i = 0; i < argc; i++)
		pkt[7 + i] = args[i];

	fid = xbs2_emit(pkt, length + 4);

	return fid;
}
