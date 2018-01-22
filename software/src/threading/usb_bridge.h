#ifndef USB_BRIDGE_H
#define USB_BRIDGE_H

#include "threading/mutex_queue.h"

#include <stdbool.h>

#define MS_TO_NANO(ms) ((ms) * 1000000)

#ifndef USB_BRIDGE_SLEEP_TIME_MS
	#define USB_BRIDGE_SLEEP_TIME_MS MS_TO_NANO(100)
#endif

extern queue_t xproc_cmd_list;

_Bool
usb_bridge_init(int iface, int vid, int pid, int baudrate);

void
usb_bridge_destroy(void);

void *
usb_bridge_monitor(void *arg);

#endif
