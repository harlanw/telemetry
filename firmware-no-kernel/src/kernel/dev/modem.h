#ifndef MODEM_H
#define MODEM_H

#include <stdint.h>

typedef enum modem_query
{
	MODEM_STATUS = 0x00
} modem_query_t;

typedef enum modem_netstat
{
	MODEM_JOINED = 0x00,
	MODEM_NO_PAN,
	MODEM_DENIED,
	MODEM_UNEXPECTED,
	MODEM_FAILED,
	MODEM_SEARCHING,
	MODEM_NO_RESP,
	MODEM_NO_KEY,
	MODEM_INVALID_KEY,
	MODEM_INITIALIZING

} modem_netstat_t;

typedef struct modem
{
	modem_netstat_t connection;
	int8_t (*connect)(char psk[], uint16_t size);
	int8_t (*update)(void);
	void (*query)(modem_query_t q);
	void (*write)(char msg[], uint16_t size);
} modem_t;

void
modem_create(modem_t *m);

#endif
