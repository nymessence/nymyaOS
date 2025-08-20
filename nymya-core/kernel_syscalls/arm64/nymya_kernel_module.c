// kernel_syscalls/nymya_syscalls.c

#include "nymya.h"

// This file exists only to generate nymya_syscalls.o as the main module object.
// Actual implementations are in other files linked as sub-objects.
#ifdef __KERNEL__
#include <linux/module.h>
MODULE_LICENSE("GPL");
#endif
