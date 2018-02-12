#include "kernel/sch/semaphore.h"

static semaphore_t semaphores[5];
static uint8_t semaphore_next = 0;

semaphore_t *
semaphore_init(int8_t value)
{
	semaphore_t *s = &semaphores[semaphore_next++];
	s->value = value;

	return s;
}

void
semaphore_post(semaphore_t *s)
{
	
}

void
semaphore_pend(semaphore_t *s)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		int8_t val = s->value;
		if (val != -128)
		{
			val--;

			if (val < 0)
			{
				ktask_current->status_pointer = s;
				ktask_current->status = TASK_SEMAPHORE;

				ktask_schedule();
			}
		}
	}
}
