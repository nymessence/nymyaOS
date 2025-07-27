// src/nymya_event_class_syscall_exit.c
//
// Defines the event_class_syscall_exit function for both kernel and userland.
// In the kernel, it uses pr_info for logging. In userland, it uses printf.

#include "nymya.h" // Assumed to define common types like uint64_t

#ifdef __KERNEL__
    #include <linux/kernel.h> // For pr_info
    #include <linux/types.h>  // For uint64_t if not already in nymya.h
    #include <linux/module.h> // Required for EXPORT_SYMBOL_GPL

    /**
     * event_class_syscall_exit - Kernel-side implementation for syscall exit event logging.
     * @syscall_id: The ID of the syscall being exited.
     * @return_code: The return code of the syscall.
     *
     * This function provides a basic logging mechanism for syscall exits within the kernel.
     * It uses pr_info to output a message to the kernel log.
     *
     * Returns: 0 on success.
     */
    int event_class_syscall_exit(uint64_t syscall_id, int return_code) {
        // For kernel logging (pr_info), %llu is generally correct for uint64_t
        pr_info("NYMYA_SYSCALL_EXIT: SyscallID=%llu, ReturnCode=%d\n", syscall_id, return_code);
        return 0;
    }
EXPORT_SYMBOL_GPL(nymya_event_class_syscall_exit);



    // Export the symbol so other kernel modules/code can call it directly.
    EXPORT_SYMBOL_GPL(nymya_event_class_syscall_exit);

#else // Userland implementation

    #include <stdio.h>    // For printf
    #include <stdint.h>   // For uint64_t
    #include <inttypes.h> // For PRIu64 macro

    /**
     * event_class_syscall_exit - Userland implementation for syscall exit event logging.
     * @syscall_id: The ID of the syscall being exited.
     * @return_code: The return code of the syscall.
     *
     * This function provides a basic logging mechanism for syscall exits in userland.
     * It uses printf to output a message to standard output.
     * This is useful for debugging and tracing in user-space applications.
     *
     * Returns: 0 on success.
     */
    int event_class_syscall_exit(uint64_t syscall_id, int return_code) {
        // Using PRIu64 for platform-independent printing of uint64_t
        printf("USERLAND_SYSCALL_EXIT: SyscallID=%" PRIu64 ", ReturnCode=%d\n", syscall_id, return_code);
        return 0;
    }

#endif // __KERNEL__
