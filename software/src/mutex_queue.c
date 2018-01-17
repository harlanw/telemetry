#include "mutex_queue.h"

#include <stdlib.h>
#include <string.h>

static message_t *
_message(const char data[])
{
	message_t *result = malloc(sizeof *result);

	result->next = NULL;
	result->size = strlen(data);
	result->data = data;

	return result;
}

int
enqueue(queue_t *const q, const char data[])
{
	message_t *msg;

	if (!q || !data)
		return 0;

	pthread_mutex_lock(&q->lock);

	msg = _message(data);

	if (!q->oldest)
	{
		q->newest = q->oldest = msg;
	}
	else
	{
		q->newest = q->newest->next = msg;
	}

	pthread_mutex_unlock(&q->lock);

	return msg->size;
}

const char *
dequeue(queue_t *const q)
{
	const char *data;
	message_t *next_oldest;

	if (!q || !q->oldest)
		return NULL;

	pthread_mutex_lock(&q->lock);

	next_oldest = q->oldest->next;
	data = q->oldest->data;
	free(q->oldest);
	q->oldest = next_oldest;

	pthread_mutex_unlock(&q->lock);

	return data;
}
