// src/nymya_3341_cz_swap.c
//
// This file implements the nymya_3341_cz_swap syscall, which applies a
// two-qubit CZ-SWAP gate. This gate is a composite operation combining a
// Controlled-Z (CZ) gate and a SWAP gate, typically used to achieve specific
// entanglement patterns or as a building block in larger quantum circuits.
//
// The implementation includes both userland and kernel-space versions,
// ensuring proper header inclusion and fixed-point arithmetic for kernel operations.

#include "nymya.h" // Common definitions like complex_double, nymya_qubit, and gate macros

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>    // Userland: For standard I/O (e.g., in log_symbolic_event)
#define __NR_nymya_3341_cz_swap NYMYA_CZ_SWAP_CODE

    #include <stdlib.h>  // Userland: For general utilities
    #include <math.h>    // Userland: For complex math functions
    #include <complex.h> // Userland: For _Complex double type
#else
    #include <linux/kernel.h>   // Kernel: For pr_err and general kernel functions
    #include <linux/syscalls.h> // Kernel: For SYSCALL_DEFINE macros
    #include <linux/uaccess.h>  // Kernel: For copy_from_user, copy_to_user
    #include <linux/errno.h>    // Kernel: For error codes like -EINVAL, -EFAULT
    #include <linux/printk.h>   // Kernel: For pr_err
    #include <linux/module.h>   // Kernel: For EXPORT_SYMBOL_GPL
    // No math.h or complex.h in kernel; fixed-point math assumed for amplitude operations
#endif

#ifndef __KERNEL__

/**
 * nymya_3341_cz_swap - Applies the CZ-SWAP gate to two qubits (userland).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function implements the CZ-SWAP gate by applying a Controlled-Z (CZ)
 * gate followed by a SWAP gate between the two qubits. This sequence results
 * in a specific entangling operation.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL (invalid input).
 */
int nymya_3341_cz_swap(nymya_qubit* q1, nymya_qubit* q2) {
    // Basic null pointer check
    if (!q1 || !q2) return -1;

    // Apply the sequence of gates that compose the CZ-SWAP gate
    nymya_3311_controlled_z(q1, q2); // Apply Controlled-Z gate
    nymya_3313_swap(q1, q2);         // Apply SWAP gate

    // Log the symbolic event for traceability
    log_symbolic_event("CZ_SWAP", q1->id, q1->tag, "CZ+SWAP applied");
    return 0;
}

#else // __KERNEL__

/**
 * nymya_3341_cz_swap - Applies the CZ-SWAP gate to two qubits (kernel-space).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function implements the CZ-SWAP gate by applying a Controlled-Z (CZ)
 * gate followed by a SWAP gate between the two qubits in kernel space.
 * This sequence results in a specific entangling operation.
 *
 * Returns:
 * - 0 on success.
 * - Error code from underlying gate operations (e.g., -EINVAL from nymya_3311_controlled_z).
 */
int nymya_3341_cz_swap(struct nymya_qubit *q1, struct nymya_qubit *q2) {
    int ret;

    // Apply the sequence of gates that compose the CZ-SWAP gate
    ret = nymya_3311_controlled_z(q1, q2); // Apply Controlled-Z gate
    if (ret) {
        pr_err("nymya_3341_cz_swap: Controlled-Z failed, error %d\n", ret);
        return ret;
    }

    ret = nymya_3313_swap(q1, q2);         // Apply SWAP gate
    if (ret) {
        pr_err("nymya_3341_cz_swap: SWAP failed, error %d\n", ret);
        return ret;
    }

    // Log the symbolic event for traceability
    log_symbolic_event("CZ_SWAP", q1->id, q1->tag, "CZ+SWAP applied");
    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3341_cz_swap);



/**
 * SYSCALL_DEFINE2(nymya_3341_cz_swap) - Kernel syscall for CZ-SWAP gate.
 * @user_q1: User-space pointer to the first qubit structure.
 * @user_q2: User-space pointer to the second qubit structure.
 *
 * This syscall copies qubit data from user space to kernel space, applies the
 * CZ-SWAP gate logic using the dedicated kernel-space function `nymya_3341_cz_swap`,
 * and then copies the modified data back to user space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user qubit pointer is NULL.
 * - -EFAULT if copying data to/from user space fails.
 * - Error code from the underlying kernel-space `nymya_3341_cz_swap` function.
 */
SYSCALL_DEFINE2(nymya_3341_cz_swap,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2; // Kernel-space copies of qubits
    int ret = 0; // Return value for syscall

    // 1. Check for null pointers from user-space
    if (!user_q1 || !user_q2) {
        pr_err("nymya_3341_cz_swap: Null user qubit pointer(s)\n");
        return -EINVAL; // Invalid argument
    }

    // 2. Copy the qubit structures from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3341_cz_swap: Failed to copy k_q1 from user\n");
        return -EFAULT; // Bad address
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3341_cz_swap: Failed to copy k_q2 from user\n");
        return -EFAULT; // Bad address
    }

    // 3. Apply the CZ-SWAP gate logic using the extracted kernel-space function
    ret = nymya_3341_cz_swap(&k_q1, &k_q2);
    if (ret) {
        // Error already logged by the core nymya_3341_cz_swap function
        return ret;
    }

    // 4. Copy the modified qubits back to user space
    // Note: k_q1 and k_q2 are both modified by CZ and SWAP, so both need to be copied back.
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3341_cz_swap: Failed to copy k_q1 to user\n");
        ret = -EFAULT; // Bad address
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3341_cz_swap: Failed to copy k_q2 to user\n");
        ret = -EFAULT; // Bad address
    }

    return ret; // Return 0 on success, or error code if any operation failed
}

#endif
