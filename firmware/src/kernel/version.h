/**
 * @file version.h
 * @author Harlan J. Waldrop <waldroha@oregonstate.edu>
 * @date 2018-01-21
 * @brief Kernel version information
 */
#ifndef VERSION_H
#define VERSION_H

/**
 * The kernel version is stored in a single 16-bit word. Each nibble corresponds to a kernel
 * subversion: major, minor, patch, in that order. The bottom nibble is reserved for future use.
 */
#define KERNEL_VERSION	0x0010

#define KERNEL_VERSION_MAJOR	(((KERNEL_VERSION) >> 12) & 0x0F)
#define KERNEL_VERSION_MINOR	(((KERNEL_VERSION) >>  8) & 0x0F)
#define KERNEL_VERSION_PATCH	(((KERNEL_VERSION) >>  4) & 0x0F)
#define KERNEL_VERSION_MAGIC	(((KERNEL_VERSION) >>  0) & 0x0F)

#endif
