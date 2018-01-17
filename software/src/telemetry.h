#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "mutex_queue.h"

#include <stdbool.h>

extern queue_t xproc_cmd_list;

_Bool
telemetry_init(int iface, int vid, int pid, int baudrate);

void
telemetry_destroy();

void *
telemetry_monitor(void *arg);

#endif
