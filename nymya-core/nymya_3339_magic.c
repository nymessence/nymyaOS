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
    #include <stdio.h>    // Userland: For standard I/O (e.g., in log_symbolic_event)
    #include <stdlib.h>  // Userland: For general utilities
    #include <math.h>    // Userland: For complex math functions
    #include <complex.h> // Userland: For _Complex double type
#else
    #include <linux/kernel.h>   // Kernel: For pr_err and general kernel functions
    #include <linux/syscalls.h> // Kernel: For SYSCALL_DEFINE macros
    #include <linux/uaccess.h>  // Kernel: For copy_from_user, copy_to_user
    #include <linux/errno.h>    // Kernel: For error codes like -EINVAL, -EFAULT
    // No math.h or complex.h in kernel; fixed-point math assumed for amplitude operations
#endif

#ifndef __KERNEL__

/**
 * nymya_3339_magic - Applies the Magic gate to two qubits (userland).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function implements the Magic gate using a decomposition into
 * Hadamard (H), S (Phase), and CNOT gates. The sequence is typically:
 * H(q1) -> S(q1) -> CNOT(q1, q2) -> H(q1).
 * This gate is known for its properties in quantum computation.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL (invalid input).
 */
int nymya_3339_magic(nymya_qubit* q1, nymya_qubit* q2) {
    // Basic null pointer check
    if (!q1 || !q2) return -1;

    // Apply the sequence of gates that compose the Magic gate
    hadamard(q1);   // Apply Hadamard to q1
    phase_s(q1);    // Apply S (Phase) gate to q1 (nymya_3306_phase_gate)
    cnot(q1, q2);   // Apply CNOT with q1 as control, q2 as target
    hadamard(q1);   // Apply Hadamard to q1 again

    // Log the symbolic event for traceability
    log_symbolic_event("MAGIC", q1->id, q1->tag, "Magic gate applied");
    return 0;
}

#else // __KERNEL__

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

    // 3. Apply the Magic gate logic for kernel space
    // Call the kernel versions of the gates
    ret = nymya_3308_hadamard_gate(&k_q1); // Hadamard on q1
    if (ret) {
        pr_err("nymya_3339_magic: Hadamard on q1 failed, error %d\n", ret);
        return ret;
    }

    ret = nymya_3306_phase_gate(&k_q1); // S (Phase) gate on q1
    if (ret) {
        pr_err("nymya_3339_magic: Phase (S) on q1 failed, error %d\n", ret);
        return ret;
    }

    ret = nymya_3309_controlled_not(&k_q1, &k_q2); // CNOT with q1 as control, q2 as target
    if (ret) {
        pr_err("nymya_3339_magic: CNOT failed, error %d\n", ret);
        return ret;
    }

    ret = nymya_3308_hadamard_gate(&k_q1); // Hadamard on q1 again
    if (ret) {
        pr_err("nymya_3339_magic: Second Hadamard on q1 failed, error %d\n", ret);
        return ret;
    }

    // 4. Log the symbolic event for traceability
    log_symbolic_event("MAGIC", k_q1.id, k_q1.tag, "Magic gate applied");

    // 5. Copy the modified qubits back to user space
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

