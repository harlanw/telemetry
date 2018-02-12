/**
 * @file attributes.h
 * @author Harlan J. Waldrop <waldroha@oregonstate.edu>
 * @date 2018-01-21
 * @brief Compiler attribute definitions
 *
 * @warning Currently only supports gcc
 *
 * @see https://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
 */
#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#ifndef __GNUC__
	#error "Compiler not supported"
	#define ATTR(attrs) ((void) 0)
#else
	/**
	 * @brief Automatic attribute definition/expansion for one or more attributes
	 */
	#define ATTR(attrs, ...) __attribute__((attrs, ##__VA_ARGS__))

	/**
	 * @brief Indicates a function that should run before main
	 */
	#define ATTR_CTOR	ATTR(constructor)

	/**
	 * @brief Indicates a function that should run after main
	 */
	#define ATTR_DTOR	ATTR(destructor)

	/**
	 * @brief Forces the compiler to inline the function
	 */
	#define ATTR_INLINE	ATTR(always_inline)

	/**
	 * @brief Informs the compiler that no paramters should be NULL
	 */
	#define ATTR_NONNULL	ATTR(nonnull)

	/**
	 * @brief Indicates a function that never returns
	 */
	#define ATTR_NORETURN	ATTR(noreturn)
#endif

#endif
