#ifndef TERMINAL_ERROR_H
#define TERMINAL_ERROR_H

#include "terminal/terminal.h"

#define ERROR(fmt, ...) \
	terminal_printf(COLOR_FGRED, 0, ">> error: " fmt NL, ##__VA_ARGS__)

#endif
