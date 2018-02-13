#ifndef XBEE_S2_H
#define XBEE_S2_H

#include "kernel/dev/modem.h"

#include <stdint.h>

typedef enum xbs2_frame_type
{
	XBS2_AT_CMD		= 0x08,
	XBS2_AT_CMD_QPARAM	= 0x09,
	XBS2_ZIG_TRANSMIT_REQ	= 0x10,
	XBS2_AT_CMD_RESP	= 0x88,
	XBS2_MODEM_STATUS	= 0x8A,
	XBS2_TRANSMIT_STATUS	= 0x8B,
} xbs2_frame_type_t;

typedef enum xbs2_mstat
{
	XBS2_HARDWARE_RESET	= 0x00,
	XBS2_WATCHDOG_RESET	= 0x01,
	XBS2_JOINED_NETWORK	= 0x02,
	XBS2_DISASSOCIATED	= 0x03,
	XBS2_COORD_STARTED	= 0x06,
	XBS2_NETKEY_UPDATED	= 0x07,
	XBS2_VOLTAGE_EXCEEDED	= 0x0D,
	XBS2_CONFIG_CHANGED	= 0x11,
	XBS2_EMBER_STACK_ERROR	= 0x80
} xbs2_mstat_t;

typedef struct xbs2_frame
{
	xbs2_frame_type_t type;
	uint16_t size;
	uint8_t id;
	uint8_t chksum;
	uint8_t pkt[30];
} xbs2_frame_t;

void
xbs2_init(modem_t *m);

int8_t
xbs2_connect(char psk[], uint16_t size);

int8_t
xbs2_update(void);

void
xbs2_query(modem_query_t q);

void
xbs2_write(char msg[], uint16_t length);

xbs2_frame_t
xbs2_get_frame_by_id(uint8_t id);

uint8_t
xbs2_emit(uint8_t pkt[], uint16_t length);

uint8_t
xbs2_at(uint8_t cmd [static 2],
	uint8_t args[],
	uint8_t argc);

#endif
