// src/nymya_event_class_syscall_exit.c
//
// Defines the nymya_event_class_syscall_exit function for both kernel and userland.
// In the kernel, it uses pr_info for logging. In userland, it uses printf.

#include "nymya.h" // Assumed to define common types like uint64_t and declare the function prototype

#ifdef __KERNEL__
    #include <linux/kernel.h> // For pr_info
    #include <linux/types.h>  // For uint64_t if not already in nymya.h
    #include <linux/module.h> // Required for MODULE_LICENSE and EXPORT_SYMBOL_GPL

    MODULE_LICENSE("GPL");

    /**
     * nymya_event_class_syscall_exit - Kernel-side implementation for syscall exit event logging.
     * @syscall_id: The ID of the syscall being exited.
     * @return_code: The return code of the syscall.
     *
     * This function provides a basic logging mechanism for syscall exits within the kernel.
     * It uses pr_info to output a message to the kernel log.
     *
     * Returns: 0 on success.
     */
    int nymya_event_class_syscall_exit(uint64_t syscall_id, uint64_t return_code) {
        pr_info("NYMYA_SYSCALL_EXIT: SyscallID=%llu, ReturnCode=%llu\n", syscall_id, return_code);
        return 0;
    }

    // Export the symbol so other kernel modules/code can call it directly.
    EXPORT_SYMBOL_GPL(nymya_event_class_syscall_exit);

#else // Userland implementation

    #include <stdio.h>    // For printf
    #include <stdint.h>   // For uint64_t
    #include <inttypes.h> // For PRIu64 macro

    /**
     * nymya_event_class_syscall_exit - Userland implementation for syscall exit event logging.
     * @syscall_id: The ID of the syscall being exited.
     * @return_code: The return code of the syscall.
     *
     * This function provides a basic logging mechanism for syscall exits in userland.
     * It uses printf to output a message to standard output.
     * This is useful for debugging and tracing in user-space applications.
     *
     * Returns: 0 on success.
     */
    int nymya_event_class_syscall_exit(uint64_t syscall_id, uint64_t return_code) {
        printf("USERLAND_SYSCALL_EXIT: SyscallID=%" PRIu64 ", ReturnCode=%" PRIu64 "\n", syscall_id, return_code);
        return 0;
    }

#endif // __KERNEL__

