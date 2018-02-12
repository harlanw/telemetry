#include "kernel/sch/tasks.h"

#include <stddef.h>
#include <stdio.h>
#include <util/atomic.h>

static ktask_t ktasks[KTASKS_MAX + 1];

static uint8_t ktask_next = 0;
static ktask_t *ktask_head = NULL;

ktask_t *ktask_current = NULL;

static uint8_t ktask_idle_stack[128];

static void
ktask_idle(void)
{
	puts("Entiner idle");
	while (1)
	{
	}
}

void
ktask_init(void)
{
	ktask_add(&ktask_idle, &ktask_idle_stack[127]);
}

void
ktask_add(ktask_fn_t fn, void *spe)
{
	ktask_t *kt;
	uint8_t *stack = spe;

	stack[0] = (uint16_t) fn & 0xFF;
	stack[-1] = (uint16_t) fn >> 8;

	for (int i = -2; i > -34; i--)
	{
		stack[i] = 0;
	}

	stack[-34] = 0x80;

	kt = &ktasks[ktask_next++];
	kt->sp = stack - 35;
	kt->status = TASK_READY;
	kt->next = NULL;

	if (ktask_head)
	{
		kt->next = ktask_head;
	}

	ktask_head = kt;

	printf("added kt: %p (%p -> %p)\n", kt, kt->sp, fn);
}

void
ktask_schedule(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		ktask_t *kt = ktask_head;

		while (kt->status != TASK_READY)
		{
			kt = kt->next;
		}

		if (kt != ktask_current)
		{
			ktask_dispatch(kt);
		}
	}
}
