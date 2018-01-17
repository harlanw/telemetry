#include "telemetry.h"

#include "mutex_queue.h"
#include "terminal/colors.h"
#include "terminal/error.h"
#include "terminal/terminal.h"

#include <time.h>
#include <libftdi1/ftdi.h>

struct ftdi_context *ftdi;
struct ftdi_version_info version;

_Bool
telemetry_init(int iface, int vid, int pid, int baudrate)
{
	int status;

	ftdi = ftdi_new();
	if (ftdi == NULL)
	{
		return false;
	}

	version = ftdi_get_library_version();
	terminal_writef(BOLD "ftdi - " RESET "initialized libftdi %d.%d.%d-%s" NL,
		version.major,
		version.minor,
		version.micro,
		version.snapshot_str);

	ftdi_set_interface(ftdi, iface);
	status = ftdi_usb_open(ftdi, vid, pid);
	if (status < 0)
	{
		ERROR("unable to open device (0x%X, 0x%X)\n"
			">> reason: %s\n"
			">> status: %d",
			vid,
			pid,
			ftdi_get_error_string(ftdi),
			status);
		ftdi_free(ftdi);

		return false;
	}

	if (ftdi->type == TYPE_R)
	{
		unsigned int chipid;

		ftdi_read_chipid(ftdi, &chipid);
		terminal_writef(BOLD "ftdi - " RESET "connected to chip with id 0x%X\n", chipid);
	}

	status = ftdi_set_baudrate(ftdi, baudrate);
	if (status < 0)
	{
		ERROR("unable to set baudrate for device (0x%X, 0x%X):\n"
			">> reason: %s\n"
			">> status: %d\n",
			vid,
			pid,
			ftdi_get_error_string(ftdi),
			status);
		ftdi_free(ftdi);

		return false;
	}

	status = ftdi_set_line_property(ftdi, BITS_8, STOP_BIT_1, NONE);
	if (status < 0)
	{
		ERROR("unable to set line parameters for device (0x%X, 0x%X):\n"
			">> reason: %s\n"
			">> status: %d\n",
			vid,
			pid,
			ftdi_get_error_string(ftdi),
			status);
		ftdi_free(ftdi);

		return false;
	}

	return true;
}

void
telemetry_destroy(void)
{
	ftdi_free(ftdi);
}

void *
telemetry_monitor(void *arg)
{
	int read;
	char buffer[1024];
	struct timespec ts;

	ts.tv_sec = 0;
	ts.tv_nsec = 100000000;

	while (true)
	{
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wpointer-sign"
		read = ftdi_read_data(ftdi, buffer, sizeof(buffer));
		#pragma GCC diagnostic pop

		if (read < 0)
		{
			nanosleep(&ts, NULL);
		}
		else if (read > 0)
		{
			terminal_write("\r" BOLD "kernel - " RESET, sizeof("\r" BOLD "kernel - " RESET));
			terminal_write(buffer, read);
			terminal_write("$ ", sizeof("$ "));
		}
	}

	return NULL;
}
