#include <avr/io.h>

.extern ktask_current

.global ktask_dispatch


.section .text

/**
 * BRIEF
 * 	Saves the current context and switches to the next context as determined
 * 	by the scheduler.
 *
 * DESCRIPTION
 * 	STACK_FRAME
 * 		[...]	<-- ktask_current->sp (general)
 * 		SREG
 * 		R31-0	<-- ktask_current->sp (base)
 * 		PCH
 * 		PCL	<-- TOP
 *
 * NOTICE
 * 	This is a critical section. It is not safe. Do not call this function
 * 	without atomic blocking.
 *
 * SEE
 * 	ktask_schedule
 */
ktask_dispatch:
	PUSH	r0
	PUSH	r1
	PUSH	r2
	PUSH	r3
	PUSH	r4
	PUSH	r5
	PUSH	r6
	PUSH	r7
	PUSH	r8
	PUSH	r9
	PUSH	r10
	PUSH	r11
	PUSH	r12
	PUSH	r13
	PUSH	r14
	PUSH	r15
	PUSH	r16
	PUSH	r17
	PUSH	r18
	PUSH	r19
	PUSH	r20
	PUSH	r21
	PUSH	r22
	PUSH	r23
	PUSH	r24
	PUSH	r25
	PUSH	r26
	PUSH	r27
	PUSH	r28
	PUSH	r29
	PUSH	r30
	PUSH	r31
	in	r0,	_SFR_IO_ADDR(SREG)
	PUSH	r0

	lds	r26,	ktask_current
	lds	r27,	ktask_current + 1

	SBIW	r26,	0
	BREQ	do_dispatch

	IN	r0,	_SFR_IO_ADDR(SPL)
	ST	X+,	r0
	IN	r0,	_SFR_IO_ADDR(SPH)
	ST	X,	r0

do_dispatch:
	MOV	r26,	r24
	MOV	r27,	r25
	STS	ktask_current,	r26
	STS	ktask_current + 1,	r27

	LD	r0,	X+
	OUT	_SFR_IO_ADDR(SPL),	r0
	LD	r0,	X
	OUT	_SFR_IO_ADDR(SPH),	r0

	POP	r0
	OUT	_SFR_IO_ADDR(SREG),	r0

	POP	r31
	POP	r30
	POP	r29
	POP	r28
	POP	r27
	POP	r26
	POP	r25
	POP	r24
	POP	r23
	POP	r22
	POP	r21
	POP	r20
	POP	r19
	POP	r18
	POP	r17
	POP	r16
	POP	r15
	POP	r14
	POP	r13
	POP	r12
	POP	r11
	POP	r10
	POP	r9
	POP	r8
	POP	r7
	POP	r6
	POP	r5
	POP	r4
	POP	r3
	POP	r2
	POP	r1
	POP	r0

	RETI
