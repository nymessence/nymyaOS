// src/nymya_3346_triangular_lattice.c
//
// This file implements the nymya_3346_triangular_lattice syscall, which applies
// a sequence of operations to three qubits to simulate an interaction pattern
// often found in triangular lattice configurations. This typically involves
// entangling operations between adjacent qubits in the "triangle".
//
// The implementation includes both userland and kernel-space versions,
// ensuring proper header inclusion and fixed-point arithmetic for kernel operations.

#include "nymya.h" // Common definitions like nymya_qubit, and gate macros

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>    // Userland: For standard I/O (e.g., in log_symbolic_event)
#define __NR_nymya_3346_triangular_lattice NYMYA_TRIANGULAR_LATTICE_CODE

    #include <stdlib.h>  // Userland: For general utilities
    #include <math.h>    // Userland: For complex math functions (if needed by sub-gates)
    #include <complex.h> // Userland: For _Complex double type (if needed by sub-gates)
#else
    #include <linux/kernel.h>   // Kernel: For pr_err and general kernel functions
    #include <linux/syscalls.h> // Kernel: For SYSCALL_DEFINE macros
    #include <linux/uaccess.h>  // Kernel: For copy_from_user, copy_to_user
    #include <linux/errno.h>    // Kernel: For error codes like -EINVAL, -EFAULT
    #include <linux/printk.h>   // Kernel: For pr_err
    // No math.h or complex.h in kernel; fixed-point math assumed for amplitude operations
#endif

#ifndef __KERNEL__

/**
 * nymya_3346_triangular_lattice - Applies operations to three qubits in a triangular lattice (userland).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @q3: Pointer to the third qubit.
 *
 * This function simulates a triangular lattice interaction by applying a sequence
 * of gates: Hadamard on q1, then CNOT(q1, q2), CNOT(q2, q3), and CNOT(q3, q1).
 * This sequence creates entanglement across the three qubits in a cyclic manner.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL (invalid input).
 */
int nymya_3346_triangular_lattice(nymya_qubit* q1, nymya_qubit* q2, nymya_qubit* q3) {
    // Basic null pointer checks
    if (!q1 || !q2 || !q3) return -1;

    // Apply the sequence of gates to simulate triangular lattice interaction
    hadamard(q1);   // Hadamard on q1
    cnot(q1, q2);   // CNOT with q1 as control, q2 as target
    cnot(q2, q3);   // CNOT with q2 as control, q3 as target
    cnot(q3, q1);   // CNOT with q3 as control, q1 as target

    // Log the symbolic event for traceability
    log_symbolic_event("TRI_LATTICE", q1->id, q1->tag, "Triangle lattice formed");
    return 0;
}

#else // __KERNEL__

/**
 * SYSCALL_DEFINE3(nymya_3346_triangular_lattice) - Kernel syscall for triangular lattice operation.
 * @user_q1: User-space pointer to the first qubit structure.
 * @user_q2: User-space pointer to the second qubit structure.
 * @user_q3: User-space pointer to the third qubit structure.
 *
 * This syscall copies qubit data from user space to kernel space, applies the
 * triangular lattice gate logic using kernel-space functions, and then copies
 * the modified data back to user space. The operation involves a sequence of
 * Hadamard and CNOT gates to entangle the three qubits.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user qubit pointer is NULL.
 * - -EFAULT if copying data to/from user space fails.
 * - Error code from underlying gate operations (e.g., nymya_3308_hadamard_gate).
 */
SYSCALL_DEFINE3(nymya_3346_triangular_lattice,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    struct nymya_qubit __user *, user_q3) {

    struct nymya_qubit k_q1, k_q2, k_q3; // Kernel-space copies of qubits
    int ret = 0; // Return value for syscall and gate operations

    // 1. Check for null pointers from user-space
    if (!user_q1 || !user_q2 || !user_q3) {
        pr_err("nymya_3346_triangular_lattice: Null user qubit pointer(s)\n");
        return -EINVAL; // Invalid argument
    }

    // 2. Copy the qubit structures from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3346_triangular_lattice: Failed to copy k_q1 from user\n");
        return -EFAULT; // Bad address
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3346_triangular_lattice: Failed to copy k_q2 from user\n");
        return -EFAULT; // Bad address
    }
    if (copy_from_user(&k_q3, user_q3, sizeof(k_q3))) {
        pr_err("nymya_3346_triangular_lattice: Failed to copy k_q3 from user\n");
        return -EFAULT; // Bad address
    }

    // 3. Apply the triangular lattice logic for kernel space
    // Call the kernel versions of the gates
    ret = nymya_3308_hadamard_gate(&k_q1); // Hadamard on k_q1
    if (ret) {
        pr_err("nymya_3346_triangular_lattice: Hadamard on q1 failed, error %d\n", ret);
        return ret;
    }

    ret = nymya_3309_controlled_not(&k_q1, &k_q2); // CNOT(k_q1, k_q2)
    if (ret) {
        pr_err("nymya_3346_triangular_lattice: CNOT(q1, q2) failed, error %d\n", ret);
        return ret;
    }

    ret = nymya_3309_controlled_not(&k_q2, &k_q3); // CNOT(k_q2, k_q3)
    if (ret) {
        pr_err("nymya_3346_triangular_lattice: CNOT(q2, q3) failed, error %d\n", ret);
        return ret;
    }

    ret = nymya_3309_controlled_not(&k_q3, &k_q1); // CNOT(k_q3, k_q1)
    if (ret) {
        pr_err("nymya_3346_triangular_lattice: CNOT(q3, q1) failed, error %d\n", ret);
        return ret;
    }

    // 4. Log the symbolic event for traceability
    log_symbolic_event("TRI_LATTICE", k_q1.id, k_q1.tag, "Triangle lattice formed");

    // 5. Copy the modified qubits back to user space
    // All three qubits can be modified, so all should be copied back.
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3346_triangular_lattice: Failed to copy k_q1 to user\n");
        ret = -EFAULT; // Bad address
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3346_triangular_lattice: Failed to copy k_q2 to user\n");
        ret = -EFAULT; // Bad address
    }
    if (copy_to_user(user_q3, &k_q3, sizeof(k_q3))) {
        pr_err("nymya_3346_triangular_lattice: Failed to copy k_q3 to user\n");
        ret = -EFAULT; // Bad address
    }

    return ret; // Return 0 on success, or error code if any operation failed
}

#endif

