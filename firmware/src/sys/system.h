#ifndef SYSTEM_H
#define SYSTEM_H

#define asm __asm__
#define volatile __volatile__

#define PANIC(rsn) panic(rsn, __FILE__, __LINE__)

void
ponic(char *rsn, const char *file, int line);

#endif
