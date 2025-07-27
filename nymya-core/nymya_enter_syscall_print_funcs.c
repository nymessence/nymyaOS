// src/nymya_enter_syscall_print_funcs.c
//
// Defines the nymya_enter_syscall_print_funcs function for both kernel and userland.
// In the kernel, it uses pr_info for logging. In userland, it uses printf.

#include "nymya.h" // Assumed to define common types like uint64_t

#ifdef __KERNEL__
    int nymya_enter_syscall_print_funcs(uint64_t syscall_id, uint64_t qubit_id);

    #include <linux/kernel.h> // For pr_info
    #include <linux/types.h>  // For uint64_t if not already in nymya.h
    #include <linux/module.h> // Required for EXPORT_SYMBOL_GPL

    /**
     * nymya_enter_syscall_print_funcs - Kernel-side implementation for syscall entry event logging.
     * @syscall_id: The ID of the syscall being entered.
     * @qubit_id: The ID of the primary qubit involved (or 0 if not applicable).
     *
     * This function provides a basic logging mechanism for syscall entries within the kernel.
     * It uses pr_info to output a message to the kernel log.
     *
     * Returns: 0 on success.
     */
    int nymya_enter_syscall_print_funcs(uint64_t syscall_id, uint64_t qubit_id) {
        // For kernel logging (pr_info), %llu is generally correct for uint64_t
        pr_info("NYMYA_SYSCALL_ENTER: SyscallID=%llu, QubitID=%llu\n", syscall_id, qubit_id);
        return 0;
    }
    // Export the symbol so other kernel modules/code can call it directly.
    EXPORT_SYMBOL_GPL(nymya_enter_syscall_print_funcs);

#else // Userland implementation

    #include <stdio.h>    // For printf
    #include <stdint.h>   // For uint64_t
    #include <inttypes.h> // For PRIu64 macro

    /**
     * nymya_enter_syscall_print_funcs - Userland implementation for syscall entry event logging.
     * @syscall_id: The ID of the syscall being entered.
     * @qubit_id: The ID of the primary qubit involved (or 0 if not applicable).
     *
     * This function provides a basic logging mechanism for syscall entries in userland.
     * It uses printf to output a message to standard output.
     * This is useful for debugging and tracing in user-space applications.
     *
     * Returns: 0 on success.
     */
    int nymya_enter_syscall_print_funcs(uint64_t syscall_id, uint64_t qubit_id) {
        // Using PRIu64 for platform-independent printing of uint64_t
        printf("USERLAND_SYSCALL_ENTER: SyscallID=%" PRIu64 ", QubitID=%" PRIu64 "\n", syscall_id, qubit_id);
        return 0;
    }

#endif // __KERNEL__

