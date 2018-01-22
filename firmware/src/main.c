/**
 * @file main.c
 * @author Harlan J. Waldrop <waldroha@oregonstate.edu>
 * @date 2018-01-21
 *
 * @brief Post-kernel-initialization routines for kernel
 *
 * This file is responsible for adding and initializing any extra kernel tasks, as well as any other
 * routines required for system operation.
 *
 * @see ktask_add()
 * @see kinit()
 * @see kend()
 */
#include "kernel/sch/tasks.h"
#include "kernel/sys/system.h"

#include <stdio.h>

static uint8_t user_proc_stack[128];	/**< stack for user-defined process */

/**
 * @brief basic high-priority user process
 */
void
user_process(void)
{
	puts("Entering user_process");
	while (1)
	{
	}
}

/**
 * @brief System invocation routines
 *
 * @note On return immediately yields to kernel's end process, @kend()
 */
int
main(void)
{
	ktask_add(&user_process, &user_proc_stack[127]);
	kfinalize();

	return 0;
}
