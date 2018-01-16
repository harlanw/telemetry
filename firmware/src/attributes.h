#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#ifdef __GNUC__
	#define ATTR(attrs, ...) __attribute__((attrs, ##__VA_ARGS__))
#else
	#define ATTR(attrs) ((void) 0)
	#warning "Compiler not supported"
#endif

#define ATTR_CTOR	ATTR(constructor)
#define ATTR_INLINE	ATTR(always_inline)
#define ATTR_NONNULL	ATTR(nonnull)
#define ATTR_NORETURN	ATTR(noreturn)

#endif
