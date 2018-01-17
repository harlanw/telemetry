#ifndef YAUGHT_COMMANDS_H
#define YAUGHT_COMMANDS_H

#include <stdbool.h>

#define COMMAND_CALLBACK(fn)	static _Bool fn(const char *raw, const char *argfmt)

void
command_register(const char *basename,
		 const char *argfmt,
		 const char *desc,
		 _Bool (*callback) (const char *, const char *),
		 _Bool *status);

_Bool
command_parse(const char *src);

void
command_print();

void
command_cleanup();

#endif
