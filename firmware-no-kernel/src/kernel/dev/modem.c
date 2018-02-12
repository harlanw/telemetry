#include "kernel/dev/modem.h"

#include "kernel/driver/xbs2.h"

void
modem_create(modem_t *m)
{
	if (m)
	{
		xbs2_init(m);
		m->connection = MODEM_FAILED;
		m->connect = xbs2_connect;
		m->update = xbs2_update;
		m->query = xbs2_query;
		m->write = xbs2_write;
	}
}
