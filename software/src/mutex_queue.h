#ifndef MUTEX_QUEUE_H
#define MUTEX_QUEUE_H

#include <pthread.h>

#define QUEUE_INITIALIZER { PTHREAD_MUTEX_INITIALIZER, \
			    PTHREAD_COND_INITIALIZER, \
			    NULL, \
			    NULL }

typedef struct message
{
	struct message *next;
	size_t size;
	const char *data;
} message_t;

typedef struct queue
{
	pthread_mutex_t lock;
	pthread_cond_t more;
	message_t *newest, *oldest;
} queue_t;

int
enqueue(queue_t *const q, const char data[]);

const char *
dequeue(queue_t *const q);

#endif
