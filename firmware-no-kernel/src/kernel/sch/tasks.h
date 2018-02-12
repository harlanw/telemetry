/**
 * @file tasks.h
 * @author Harlan J. Waldrop <waldroha@oregonstate.edu>
 * @date 2018-01-21
 *
 * @brief Kernel-level multitasking with preemption.
 */
#ifndef KTASKS_H
#define KTASKS_H

#include <stdint.h>

#ifndef KTASKS_MAX
	/**
	 * @brief Max number of tasks if none provided. Does not include idle task.
	 */
	#define KTASKS_MAX 4
#endif

/**
 * @brief Task status used to determine which task to execute next
 */
typedef enum {
	TASK_READY,	/**< Task is ready to execute*/
	TAKS_SEMAPHORE,
	TASK_BUSY	/**< Task is waiting for event */
} ktask_status_t;

/**
 * @brief Task Control Block (TCB)
 *
 * @todo Add task timeout
 */
typedef struct ktask
{
	void *sp;		/**< stack pointer for corresponding task block*/
	ktask_status_t status;	/**< task status used for scheduling */
	struct ktask *next;	/**< next task to execute, always lower priority */
} ktask_t;

/**
 * @brief Function prototype for all task functions
 *
 * A function of this type is associated with a given task when ktask_add() is called. This function
 * represents the entire task. In other words, the call stack for any task will always terminate
 * here.
 */
typedef void (*ktask_fn_t)(void);

extern ktask_t *ktask_current;

/**
 * @brief Initializes the Task Control Block stack with an idle task.
 *
 * The idle task is an infinite loop with the lowest priority. It ensures that the scheduler is
 * always running a task.
 */
void
ktask_init(void);

/**
 * @brief Adds a task to the Task Control Block stack.
 *
 * This task will have higher priority than any preceding tasks.
 *
 * @param fn function the task should execute
 * @param spe pointer to the top (end) of a stack
 */
void
ktask_add(ktask_fn_t fn, void *spe);

void
ktask_schedule(void);

extern void
ktask_dispatch(ktask_t *task);

#endif
