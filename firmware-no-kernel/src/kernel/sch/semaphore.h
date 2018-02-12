#ifndef SEMAPHORE_H
#define SEMAPHORE_H

typedef struct {
	int8_t value;
} semaphore_t;

semaphore_t *
semaphore_init(int8_t value);

void
semaphore_post(semaphore_t *s);

void
semaphore_pend(semaphore_t *s);

#endif
