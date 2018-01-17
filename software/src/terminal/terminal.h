#ifndef YAUGHT_CONSOLE_H
#define YAUGHT_CONSOLE_H

#include <stdarg.h>
#include <stddef.h>

#define NL	"\033[m\n"

enum color
{
	COLOR_BGBLACK	= 0x00,
	COLOR_BGRED	= 0x01,
	COLOR_BGGREEN	= 0x02,
	COLOR_BGYELLOW	= 0x03,
	COLOR_BGBLUE	= 0x04,
	COLOR_BGMAGENTA	= 0x05,
	COLOR_BGCYAN	= 0x06,
	COLOR_BGWHITE	= 0x07,
	COLOR_BGGREY	= 0x08,
	COLOR_FGBLACK	= 0x00,
	COLOR_FGRED	= 0x10,
	COLOR_FGGREEN	= 0x20,
	COLOR_FGYELLOW	= 0x30,
	COLOR_FGBLUE	= 0x40,
	COLOR_FGMAGENTA	= 0x50,
	COLOR_FGCYAN	= 0x60,
	COLOR_FGWHITE	= 0x70,
	COLOR_FGGRAY	= 0x80
};
typedef enum color color_t;

enum style
{
	STYLE_NORMAL	= 0,
	STYLE_BOLD	= 1,
	STYLE_DIM	= 2,
	STYLE_ITALIC	= 3,
	STYLE_UNDERLINE	= 4,
	STYLE_BLINK	= 5,
	STYLE_DEFAULT	= 6,
	STYLE_INVERT	= 7,
	STYLE_HIDDEN	= 8,
	STYLE_STRIKE	= 9
};
typedef enum style style_t;

void
terminal_init(void);

void
terminal_destroy(void);

void
terminal_cooked(void);

void
terminal_raw(void);

char *
terminal_get(void);

void
terminal_clear(void);

void
terminal_break();

void
terminal_set(color_t color, style_t style);

void
terminal_center(const char *str, int length);

void
terminal_write(const char *str, size_t length);

void
terminal_print(color_t color, style_t style, const char *str, size_t length);

void
terminal_writef(const char *fmt, ...);

void
terminal_printf(color_t color, style_t style, const char *fmt, ...);

void
terminal_writeline(color_t color, style_t style, size_t length);

#endif
