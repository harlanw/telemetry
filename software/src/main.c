#include "attributes.h"
#include "commands.h"
#include "telemetry.h"
#include "mutex_queue.h"
#include "terminal/error.h"
#include "terminal/terminal.h"

#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libftdi1/ftdi.h>

#define TELEMETRY_VERSION_STR	"0.0.1"
#define TELEMETRY_BANNER	"Telemetry (" TELEMETRY_VERSION_STR ")"

#define FTDI_VENDOR_ID 0x403
#define FTDI_PRODUCT_ID 0x6001
#define FTDI_DEFAULT_BAUDRATE 9600

queue_t xproc_cmd_list = QUEUE_INITIALIZER;
static pthread_t thread_id;

static _Bool
parse_args(int argc, char *argv[])
{
	_Bool result;

	int iface;
	int vid, pid;
	int baudrate;

	int i;

	result;

	iface = INTERFACE_ANY;
	vid = FTDI_VENDOR_ID;
	pid = FTDI_PRODUCT_ID;
	baudrate = FTDI_DEFAULT_BAUDRATE;

	while ((i = getopt(argc, argv, "i:v:p:b:")) != -1)
	{
		switch (i)
		{
			case 'i':
				iface = atoi(optarg);
				break;
			case 'v':
				vid = atoi(optarg);
				break;
			case 'p':
				pid = atoi(optarg);
				break;
			case 'b':
				baudrate = atoi(optarg);
				break;
		}
	}

	result = telemetry_init(iface, vid, pid, baudrate);

	return result;
}

static inline void ATTR_INLINE
die_gracefully(void)
{
	pthread_join(thread_id, NULL);
	terminal_destroy();
	terminal_cooked();
	command_cleanup();
	telemetry_destroy();
}

void ATTR_NORETURN
handle_signal(int ATTR_UNUSED signal)
{
	die_gracefully();

	exit(EXIT_SUCCESS);
}

COMMAND_CALLBACK(help_command_cb)
{
	command_print();

	return true;
}

COMMAND_CALLBACK(ping_command_cb)
{
	return true;
}

COMMAND_CALLBACK(quit_command_cb)
{
	return false;
}

int
main(int argc, char *argv[])
{
	int thread_err;
	_Bool should_continue = true;

	(void) signal(SIGINT, handle_signal);

	command_register("help","%*s", "prints this menu", help_command_cb, NULL);
	command_register("ping","%*s", "pings the telemetry board", ping_command_cb, NULL);
	command_register("quit","%*s", "exits the program", quit_command_cb, &should_continue);

	terminal_init();

	terminal_print(COLOR_FGBLUE, STYLE_BOLD, TELEMETRY_BANNER, sizeof(TELEMETRY_BANNER));
	terminal_break();

	should_continue = parse_args(argc, argv);
	if (!should_continue)
		return EXIT_FAILURE;

	thread_err = pthread_create(&thread_id, NULL, &telemetry_monitor, NULL);
	if (thread_err)
	{
		ERROR("couldn't launch telemetry monitor\n"
			">> reason: %s",
			strerror(thread_err));

		return EXIT_FAILURE;
	}

	enqueue(&xproc_cmd_list, "hey");

	while (should_continue)
	{
		char * const raw = terminal_get();
		_Bool status = command_parse(raw);

		free(raw);
	}

	die_gracefully();
}
