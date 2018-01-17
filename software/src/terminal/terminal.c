#include "terminal.h"

#include "terminal/utf8.h"

#include <ctype.h>
#include <termio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef MULTITHREAD
	#include <pthread.h>

	static pthread_mutex_t mutex;
	#define MUTEX_LOCK() pthread_mutex_lock(&mutex)
	#define MUTEX_UNLOCK() pthread_mutex_unlock(&mutex)
#else
	#define MUTEX_LOCK()	((void) 0)
	#define MUTEX_UNLOCK()	((void) 0)
#endif

#ifdef MULTITHREAD
#endif

static int head = 0;
static char last_char = 0;

static void
_echo_off(void)
{
	struct termios term;
	(void) tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~ECHO;
	(void) tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

static void
_echo_on(void)
{
	struct termios term;
	(void) tcgetattr(STDIN_FILENO, &term);
	term.c_lflag |= ECHO;
	(void) tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

static void
_raw_off(void)
{
	struct termios term;
	(void) tcgetattr(STDIN_FILENO, &term);
	term.c_lflag |= ICANON;
	(void) tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

static void
_raw_on(void)
{
	struct termios term;
	(void) tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~ICANON;
	(void) tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

static void
_blot_char(char c)
{
	if (c == '\n' || c == '\r')
		head = 0;
	else
		++head;

	if (!(last_char == '\n' && c == ' '))
	{
		last_char = c;
		(void) putchar(c);
	}

	if (c == '\r')
	{
		(void) fputs("\33[2K", stdout);
	}

	if (head >= 80)
	{
		(void) putchar('\n');
		last_char = '\n';
		head = 0;
	}

	(void) fflush(stdout);
}

static void
_blot_string(const char *str, size_t length)
{
	MUTEX_LOCK();

	if (!length)
		length = strlen(str);

	int i;
	for (i = 0; i < length; i++)
		_blot_char(str[i]);

	MUTEX_UNLOCK();
}

void
terminal_init(void)
{
#ifdef MULTITHREAD
	pthread_mutex_init(&mutex, NULL);
#endif
}

void
terminal_destroy(void)
{
#ifdef MULTITHREAD
	pthread_mutex_destroy(&mutex);
#endif
}

void
terminal_cooked(void)
{
	_echo_on();
	_raw_off();
}

void
terminal_raw(void)
{
	_echo_off();
	_raw_on();
}

char *
terminal_get(void)
{
	char *result = NULL;

	char line[256], *ptr, c;
	size_t length, ignore;

	//_blot_string("¥  ", 3);
	_blot_string("$ ", 2);
	terminal_set(0, 0);

	terminal_raw();

	ptr = line;
	length = ignore = 0;
	while ((c = getchar()) != '\n')
	{
		if (iscntrl(c))
		{
			if (c == '\033')
				ignore = 2;
			else if (c == 8 && ptr != line)
			{
				printf("\033[1D \033[1D");
				--ptr;
			}
		}
		else if (!ignore)
		{
			*(ptr++) = c;
			++length;
			printf("%c", c);
		}
		else
		{
			--ignore;
		}

		if (ptr == line + 255)
			break;
	}

	*ptr = '\0';
	result = malloc((length + 1) * sizeof *result);
	strcpy(result, line);

	terminal_cooked();

	return result;
}

void
terminal_clear(void)
{
	head = 0;
	printf("\033[2J");
	(void) fflush(stdout);
}

void
terminal_break()
{
	_blot_string(NL, sizeof(NL));
}

void
terminal_set(color_t color, style_t style)
{
	static const char *color_lut[] =
	{
		"9", "1", "2", "3", "4", "5", "6", "7", "0"
	};

	static const char *style_lut[] =
	{
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
	};

	printf("\033[%s;3%s;4%sm",
		style_lut[style],
		color_lut[(color >> 4)],
		color_lut[(color & 0x0F)]);
}

void
terminal_centerf(const char *str, int length)
{
	
}

void
terminal_center(const char *str, int length)
{
	int offs;

	if (head)
		_blot_char('\n');

	offs = (80 - length) / 2;
	if (length <= 0)
		_blot_string(str, length);
	else
	{
		last_char = 0;
		for (int i = 0; i < offs; i++)
			_blot_char(' ');

		_blot_string(str, length);
	}

	_blot_char('\n');
}

void
terminal_write(const char *str, size_t length)
{
	_blot_string(str, length);
}

void
terminal_print(color_t color, style_t style, const char *str, size_t length)
{
	terminal_set(color, style);
	_blot_string(str, length);
}

void
terminal_writef(const char *fmt, ...)
{
	va_list ap0, ap1;

	char *buffer;
	size_t length;

	va_start(ap0, fmt);
	va_copy(ap1, ap0);
	length = vsnprintf(NULL, 0, fmt, ap0);
	va_end(ap0);

	if (!length)
		return;

	buffer = malloc((length + 1) * sizeof *buffer);

	vsnprintf(buffer, length + 1, fmt, ap1);
	va_end(ap1);

	_blot_string(buffer, length);

	free(buffer);
}

void
terminal_printf(color_t color, style_t style, const char *fmt, ...)
{
	va_list ap0, ap1;

	char *buffer;
	size_t length;

	terminal_set(color, style);

	va_start(ap0, fmt);
	va_copy(ap1, ap0);
	length = vsnprintf(NULL, 0, fmt, ap0);
	va_end(ap0);

	if (!length)
		return;

	buffer = malloc((length + 1) * sizeof *buffer);

	vsnprintf(buffer, length + 1, fmt, ap1);
	va_end(ap1);

	_blot_string(buffer, length);

	free(buffer);
}

void
terminal_writeline(color_t color, style_t style, size_t length)
{
	length /= 4;

	terminal_set(color, style);

	for (int i = 0; i < length; i++)
		printf("%s %s ", UTF8_HEAVY_HORIZONTAL, UTF8_HEAVY_HORIZONTAL);
	terminal_break();

	head = 0;
}
