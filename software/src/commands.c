#include "commands.h"

#include "terminal/colors.h"
#include "terminal/terminal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct command
{
	struct command *next;

	const char *basename;
	const char *argfmt;
	const char *desc;

	_Bool (*callback) (const char *, const char *);

	_Bool *status;
};

typedef struct command command_t;

static command_t *p_cmdlist = NULL;

void
command_register(
	const char *basename,
	const char *argfmt,
	const char *desc,
	_Bool (*callback) (const char *, const char *),
	_Bool *status)
{
	command_t *curr, **lptr;
	command_t *cmd = malloc(sizeof *cmd);

	cmd->next = NULL;
	cmd->basename = basename;
	cmd->argfmt = argfmt;
	cmd->desc = desc;
	cmd->callback = callback;
	cmd->status = status;

	curr = p_cmdlist;
	lptr = &p_cmdlist;

	// Update "next" for the previous tail (now left-of tail)
	while (curr)
	{
		lptr = &(curr->next);
		curr = curr->next;
	}
	*lptr = cmd;
}

_Bool
command_parse(const char *raw)
{
	_Bool result;

	command_t *cmd;
	char basename[256];
	int matches;

	result = false;
	
	matches = sscanf(raw, "%s", basename);
	if (matches != EOF)
	{
		for (cmd = p_cmdlist; cmd != NULL; cmd = cmd->next)
			if (!strcmp(basename, cmd->basename))
			{
				terminal_writef("\033[2K\r");
				result = cmd->callback(raw, cmd->argfmt);

				if (cmd->status)
					*cmd->status = result;

				break;
			}
	}

	//terminal_writef("\033[2K\r%s\n", raw);
	// TODO reset 
	terminal_writef("\033[2K\r");
	if (result)
		terminal_set(COLOR_FGGREEN, STYLE_BOLD);
	else
		terminal_set(COLOR_FGRED, STYLE_BOLD);

	return result;
}

void
command_print()
{
	for (command_t *cmd = p_cmdlist; cmd != NULL; cmd = cmd->next)
		if (cmd->desc)
			terminal_writef(BOLD "%s" RESET " \t- %s\n", cmd->basename, cmd->desc);
}

void
command_cleanup()
{
	command_t *tmp, *curr = p_cmdlist;

	while (curr)
	{
		tmp = curr;
		curr = curr->next;

		free(tmp);
	}

	p_cmdlist = NULL;
}
