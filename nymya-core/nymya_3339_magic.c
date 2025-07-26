// src/nymya_3339_magic.c
//
// This file implements the nymya_3339_magic syscall, which applies a
// two-qubit "Magic" gate. The Magic gate is a specific entangling gate
// often used in quantum information theory, typically constructed from
// a sequence of other fundamental gates like Hadamard, S (Phase), and CNOT.
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
#define __NR_nymya_3339_magic NYMYA_MAGIC_CODE

    #include <stdlib.h>  // Userland: For general utilities
    #include <math.h>    // Userland: For complex math functions
    #include <complex.h> // Userland: For _Complex double type
#else
    #include <linux/kernel.h>   // Kernel: For pr_err and general kernel functions
    #include <linux/syscalls.h> // Kernel: For SYSCALL_DEFINE macros
    #include <linux/uaccess.h>  // Kernel: For copy_from_user, copy_to_user
    #include <linux/errno.h>    // Kernel: For error codes like -EINVAL, -EFAULT
    #include <linux/module.h>   // Kernel: Required for EXPORT_SYMBOL_GPL
    // No math.h or complex.h in kernel; fixed-point math assumed for amplitude operations

/**
 * @brief Applies the Magic gate operation to two kernel-space qubits.
 *
 * This function encapsulates the core logic for the Magic gate, decomposing
 * it into a sequence of kernel-level Hadamard, Phase (S), and CNOT gates.
 * It operates directly on kernel-space qubit structures.
 *
 * @param kq1 Pointer to the first kernel-space qubit structure.
 * @param kq2 Pointer to the second kernel-space qubit structure.
 * @return 0 on success, or a negative error code if any underlying gate
 *         operation fails.
 */
int nymya_3339_magic(struct nymya_qubit *kq1, struct nymya_qubit *kq2) {
    int ret = 0;

    // Apply the Magic gate logic for kernel space
    // Call the kernel versions of the gates
    ret = nymya_3308_hadamard_gate(kq1); // Hadamard on q1
    if (ret) {
        pr_err("nymya_3339_magic: Hadamard on q1 failed, error %d\n", ret);
        return ret;
    }

    ret = nymya_3306_phase_gate(kq1); // S (Phase) gate on q1
    if (ret) {
        pr_err("nymya_3339_magic: Phase (S) on q1 failed, error %d\n", ret);
        return ret;
    }

    ret = nymya_3309_controlled_not(kq1, kq2); // CNOT with q1 as control, q2 as target
    if (ret) {
        pr_err("nymya_3339_magic: CNOT failed, error %d\n", ret);
        return ret;
    }

    ret = nymya_3308_hadamard_gate(kq1); // Hadamard on q1 again
    if (ret) {
        pr_err("nymya_3339_magic: Second Hadamard on q1 failed, error %d\n", ret);
        return ret;
    }

    // Log the symbolic event for traceability
    log_symbolic_event("MAGIC", kq1->id, kq1->tag, "Magic gate applied");

    return 0; // Success
}
EXPORT_SYMBOL_GPL(nymya_3339_magic);


/**
 * SYSCALL_DEFINE2(nymya_3339_magic) - Kernel syscall for Magic gate.
 * @user_q1: User-space pointer to the first qubit structure.
 * @user_q2: User-space pointer to the second qubit structure.
 *
 * This syscall copies qubit data from user space to kernel space, applies the
 * Magic gate logic using kernel-space functions, and then copies the modified
 * data back to user space. The Magic gate is decomposed into kernel-level
 * Hadamard, Phase (S), and CNOT gates.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user qubit pointer is NULL.
 * - -EFAULT if copying data to/from user space fails.
 * - Error code from underlying gate operations (e.g., nymya_3308_hadamard_gate).
 */
SYSCALL_DEFINE2(nymya_3339_magic,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2; // Kernel-space copies of qubits
    int ret = 0; // Return value for syscall

    // 1. Check for null pointers from user-space
    if (!user_q1 || !user_q2) {
        pr_err("nymya_3339_magic: Null user qubit pointer(s)\n");
        return -EINVAL; // Invalid argument
    }

    // 2. Copy the qubit structures from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3339_magic: Failed to copy k_q1 from user\n");
        return -EFAULT; // Bad address
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3339_magic: Failed to copy k_q2 from user\n");
        return -EFAULT; // Bad address
    }

    // 3. Apply the Magic gate logic using the new core function
    ret = nymya_3339_magic(&k_q1, &k_q2);
    if (ret) {
        // Error already logged by the core function
        return ret;
    }

    // 4. Copy the modified qubits back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3339_magic: Failed to copy k_q1 to user\n");
        ret = -EFAULT; // Bad address
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3339_magic: Failed to copy k_q2 to user\n");
        ret = -EFAULT; // Bad address
    }

    return ret; // Return 0 on success, or error code if any operation failed
}

#endif
