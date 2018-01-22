/**
 * @file system.h
 * @author Harlan J. Waldrop <waldroha@oregonstate.edu>
 * @date 2018-01-21
 *
 * @brief Kernel system definitions
 *
 * @see kernel.c
 * @see panic.c
 */
#ifndef SYSTEM_H
#define SYSTEM_H

#ifdef __GNUC__
	#define asm __asm__
	#define volatile __volatile__

	#define PANIC(rsn) panic(rsn, __FILE__, __LINE__) /**< automatic expansion for panic() */
#else
	#define PANIC(rsn) panic(rsn, 0, -1);
#endif

#define SLEEP() asm("SLEEP");	/**< Similar to halt instruction **/

/**
 * @brief Called when the kernel encounters an fatal error
 *
 * During kernel panic a brief error message is logged and the microcontroller is halted
 *
 * @param rsn reason for failure
 * @param file file name where failure is encountered
 * @param line file line of failure
 */
void
panic(char *rsn, const char *file, int line);

/**
 * @brief Call to finalize system initialization.
 *
 * This function will enable the kernel process responsible for scheduling tasks. This means that
 * any following routines that are not proper kernel tasks will not reliably run.
 *
 * @warning Only call this function once
 */
void
kfinalize(void);

#endif
