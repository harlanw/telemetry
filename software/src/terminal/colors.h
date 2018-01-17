#ifndef YAUGHT_CONSOLE_COLORS_H
#define YAUGHT_CONSOLE_COLORS_H

#define COLOR3(w,f,b)	"\033[" w ";3" f ";4" b "m"
#define COLOR2(w,f)	"\033[" w ";3" f "m"
#define COLOR(w)	"\033[" w "m"

#define RESET		COLOR("0")
#define BOLD		COLOR("1")
#define DIM		"2"
#define UNDERLINE	"4"
#define INVERTED	"7"
#define HIDDEN		"8"

#define BLACK		"0"
#define RED		"1"
#define GREEN		"2"
#define YELLOW 		"3"
#define BLUE		"4"
#define MAGENTA		"5"
#define CYAN		"6"
#define WHITE		"7"

#endif
