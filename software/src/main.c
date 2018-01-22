///
/// @file main.c
/// @author Harlan Waldrop <root@aboyandhisgnu.org>
/// @date 2018-01-18
/// @brief tshl - an interactive shell for communicating with the telemetry board
///
/// @see https://www.intra2net.com/en/developer/libftdi/
///
#include "attributes.h"
#include "commands.h"
#include "globals.h"
#include "terminal/colors.h"
#include "terminal/error.h"
#include "terminal/terminal.h"
#include "threading/mutex_queue.h"
#include "threading/usb_bridge.h"

#include <dlfcn.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libftdi1/ftdi.h>

#define MODULE_PATH "build/modules/"

queue_t xproc_cmd_list = QUEUE_INITIALIZER;	/**< list of command messages between threads */
static pthread_t thread_id;	// FTDI communications thread

static _Bool
parse_args(int argc, char *argv[], int *iface, int *vid, int *pid, int *baudrate)
{
	int i;

	*iface = INTERFACE_ANY;
	*vid = FTDI_VENDOR_ID;
	*pid = FTDI_PRODUCT_ID;
	*baudrate = FTDI_DEFAULT_BAUDRATE;

	opterr = 0;

	while ((i = getopt(argc, argv, "i:v:p:b:")) != -1)
	{
		switch (i)
		{
			case 'i':
				*iface = atoi(optarg);
				break;
			case 'v':
				*vid = atoi(optarg);
				break;
			case 'p':
				*pid = atoi(optarg);
				break;
			case 'b':
				*baudrate = atoi(optarg);
				break;
			default:
				return false;
		}
	}

	if (argc >= optind)
		return true;
	else
		return false;

	return true;
}

static inline void ATTR_INLINE
die_gracefully(void)
{
	pthread_join(thread_id, NULL);
	terminal_destroy();
	terminal_cooked();
	command_cleanup();
	usb_bridge_destroy();
}

static void ATTR_NORETURN
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

COMMAND_CALLBACK(do_command_cb)
{
	char cmd[1024];
	int matches;

	matches = sscanf(raw, argfmt, cmd);
	if (matches == EOF)
	{
		return false;
	}

	// validate

	enqueue(&xproc_cmd_list, cmd);

	return true;
}

COMMAND_CALLBACK(quit_command_cb)
{
	return true;
}

int
main(int argc, char *argv[])
{
	int iface, vid, pid, baudrate;
	int thread_err;
	_Bool should_continue = true;

	(void) signal(SIGINT, handle_signal);

	void *handle;
	void (*fn)(void);

	handle = dlopen(MODULE_PATH "help.so", RTLD_NOW);
	if (!handle)
	{
		ERROR("%s\n", dlerror());
		return EXIT_FAILURE;
	}
	fn = dlsym(handle, "foo");
	(*fn)();
	dlclose(handle);

	// SHELL COMMANDS
	command_register("help", "%*s", "prints this menu", help_command_cb, NULL);
	command_register("do", "%*s %s", "send a command to the board", do_command_cb, NULL);
	command_register("quit", "%*s", "exits the program", quit_command_cb, &should_continue);

	// TERMINAL
	terminal_init();

	terminal_print(COLOR_FGGREEN, STYLE_BOLD, TELEMETRY_BANNER, sizeof(TELEMETRY_BANNER));
	terminal_break();

	// Parse command line args
	should_continue = parse_args(argc, argv, &iface, &vid, &pid, &baudrate);
	if (!should_continue)
	{
		terminal_writef(BOLD "usage:" RESET \
			" %s [-i iface] [-v vid] [-p pid] [-b baudrate]\n",
			*argv);

		return EXIT_FAILURE;
	}

	// Initialize FTDI bridge
	should_continue = usb_bridge_init(iface, vid, pid, baudrate);
	if (!should_continue)
		return EXIT_FAILURE;

	// Start bridge watch thread
	thread_err = pthread_create(&thread_id, NULL, &usb_bridge_monitor, NULL);
	if (thread_err)
	{
		ERROR("couldn't start usb_bridge thread\n"
			">> reason: %s",
			strerror(thread_err));

		return EXIT_FAILURE;
	}

	// Enter REPL mode
	while (should_continue)
	{
		char * const raw = terminal_get();
		_Bool status = command_parse(raw);

		free(raw);
	}

	die_gracefully();
}
