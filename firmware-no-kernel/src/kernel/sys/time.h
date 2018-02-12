#ifndef TIME_H
#define TIME_H

#include <stdint.h>

typedef struct time
{
	uint16_t ms;
	uint16_t s;
	uint16_t m;
} time_t;

#endif
