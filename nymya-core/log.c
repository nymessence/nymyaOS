// src/log.c
//
// Defines the log_symbolic_event function for both kernel and userland.
// In the kernel, it uses pr_info for logging. In userland, it uses printf.

#include "nymya.h" // Assumed to define common types like uint64_t and nymya_qubit

#ifdef __KERNEL__
    #include <linux/kernel.h> // For pr_info
    #include <linux/module.h> // For EXPORT_SYMBOL_GPL
    #include <linux/types.h>  // For uint64_t if not already in nymya.h

    /**
     * log_symbolic_event - Kernel-side implementation for logging symbolic events.
     * @gate: The name of the gate or event.
     * @id: The ID of the primary qubit involved.
     * @tag: An optional tag for the qubit.
     * @msg: A descriptive message for the event.
     *
     * This function provides a basic logging mechanism for quantum gate operations
     * or other symbolic events within the kernel. It uses pr_info to output
     * a message to the kernel log.
     *
     * Returns: 0 on success.
     */
    int log_symbolic_event(const char* gate, uint64_t id, const char* tag, const char* msg) {
        // For kernel logging (pr_info), %llu is generally correct for uint64_t
        pr_info("NYMYA_KERNEL_EVENT: [%s] Qubit ID %llu (%s): %s\n",
                gate, id, tag ? tag : "untagged", msg);
        return 0;
    }
    // Export the symbol so other kernel modules can use this logging function.
    EXPORT_SYMBOL_GPL(log_symbolic_event);

#else // Userland implementation

    #include <stdio.h>    // For printf
    #include <stdint.h>   // For uint64_t
    #include <inttypes.h> // For PRIu64 macro

    /**
     * log_symbolic_event - Userland implementation for logging symbolic events.
     * @gate: The name of the gate or event.
     * @id: The ID of the primary qubit involved.
     * @tag: An optional tag for the qubit.
     * @msg: A descriptive message for the event.
     *
     * This function provides a basic logging mechanism for quantum gate operations
     * or other symbolic events in userland. It uses printf to output a message
     * to standard output.
     *
     * Returns: 0 on success.
     */
    int log_symbolic_event(const char* gate, uint64_t id, const char* tag, const char* msg) {
        // Using PRIu64 for platform-independent printing of uint64_t
        printf("NYMYA_USERLAND_EVENT: [%s] Qubit ID %" PRIu64 " (%s): %s\n",
               gate, id, tag ? tag : "untagged", msg);
        return 0;
    }

#endif // __KERNEL__

