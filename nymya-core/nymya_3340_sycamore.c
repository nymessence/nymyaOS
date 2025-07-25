// src/nymya_3340_sycamore.c
//
// This file implements the nymya_3340_sycamore syscall, which applies a
// two-qubit Sycamore gate. The Sycamore gate is a specific entangling gate
// known from Google's quantum processors, typically constructed from a
// square root of iSWAP gate and a controlled-phase gate.
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
#define __NR_nymya_3340_sycamore NYMYA_SYCAMORE_CODE

    #include <stdlib.h>  // Userland: For general utilities
    #include <math.h>    // Userland: For M_PI and other math functions
    #include <complex.h> // Userland: For _Complex double type
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
 * nymya_3340_sycamore - Applies the Sycamore gate to two qubits (userland).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function implements the Sycamore gate, a two-qubit entangling gate.
 * It is typically decomposed into a square root of iSWAP gate followed by
 * a Controlled-Phase gate with a specific angle (e.g., PI/6).
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL (invalid input).
 */
int nymya_3340_sycamore(nymya_qubit* q1, nymya_qubit* q2) {
    // Basic null pointer check
    if (!q1 || !q2) return -1;

    // Apply the sequence of gates that compose the Sycamore gate
    nymya_3327_sqrt_iswap(q1, q2); // Apply square root of iSWAP gate
    nymya_3317_controlled_phase(q1, q2, M_PI / 6.0); // Apply Controlled-Phase gate with angle PI/6

    // Log the symbolic event for traceability
    log_symbolic_event("SYCAMORE", q1->id, q1->tag, "Sycamore gate applied");
    return 0;
}

#else // __KERNEL__

/**
 * SYSCALL_DEFINE2(nymya_3340_sycamore) - Kernel syscall for Sycamore gate.
 * @user_q1: User-space pointer to the first qubit structure.
 * @user_q2: User-space pointer to the second qubit structure.
 *
 * This syscall copies qubit data from user space to kernel space, applies the
 * Sycamore gate logic using kernel-space functions and fixed-point arithmetic,
 * and then copies the modified data back to user space. The Sycamore gate is
 * decomposed into kernel-level square root of iSWAP and Controlled-Phase gates.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user qubit pointer is NULL.
 * - -EFAULT if copying data to/from user space fails.
 * - Error code from underlying gate operations (e.g., nymya_3327_sqrt_iswap).
 */
SYSCALL_DEFINE2(nymya_3340_sycamore,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2; // Kernel-space copies of qubits
    int ret = 0; // Return value for syscall

    // 1. Check for null pointers from user-space
    if (!user_q1 || !user_q2) {
        pr_err("nymya_3340_sycamore: Null user qubit pointer(s)\n");
        return -EINVAL; // Invalid argument
    }

    // 2. Copy the qubit structures from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3340_sycamore: Failed to copy k_q1 from user\n");
        return -EFAULT; // Bad address
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3340_sycamore: Failed to copy k_q2 from user\n");
        return -EFAULT; // Bad address
    }

    // 3. Apply the Sycamore gate logic for kernel space
    // Call the kernel versions of the gates
    ret = nymya_3327_sqrt_iswap(&k_q1, &k_q2); // Apply square root of iSWAP gate
    if (ret) {
        pr_err("nymya_3340_sycamore: sqrt_iswap failed, error %d\n", ret);
        return ret;
    }

    // Convert M_PI / 6.0 to fixed-point for nymya_3317_controlled_phase
    // M_PI / 6.0 is approximately 0.5235987756 radians
    // Convert to fixed-point: (int64_t)(0.5235987756 * FIXED_POINT_SCALE)
    int64_t phase_angle_fp = (int64_t)(0.5235987756 * FIXED_POINT_SCALE);
    ret = nymya_3317_controlled_phase(&k_q1, &k_q2, phase_angle_fp); // Apply Controlled-Phase gate
    if (ret) {
        pr_err("nymya_3340_sycamore: controlled_phase failed, error %d\n", ret);
        return ret;
    }

    // 4. Log the symbolic event for traceability
    log_symbolic_event("SYCAMORE", k_q1.id, k_q1.tag, "Sycamore gate applied");

    // 5. Copy the modified qubits back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3340_sycamore: Failed to copy k_q1 to user\n");
        ret = -EFAULT; // Bad address
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3340_sycamore: Failed to copy k_q2 to user\n");
        ret = -EFAULT; // Bad address
    }

    return ret; // Return 0 on success, or error code if any operation failed
}

#endif

