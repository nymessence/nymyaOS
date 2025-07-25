// src/nymya_3345_cf_swap.c
//
// This file implements the nymya_3345_cf_swap syscall, which applies a
// Controlled-Fredkin (CF-SWAP) gate. This is a three-qubit gate where the
// swap operation between two target qubits (q1, q2) is controlled by a
// third qubit (qc). If the control qubit is in the |1> state, the target
// qubits are swapped; otherwise, no operation occurs on the targets.
//
// The implementation includes both userland and kernel-space versions,
// ensuring proper header inclusion and fixed-point arithmetic for kernel operations.

#include "nymya.h" // Common definitions like complex_double, nymya_qubit, and fixed-point helpers

#ifndef __KERNEL__
    #include <stdio.h>    // Userland: For standard I/O (e.g., in log_symbolic_event)
    #include <stdlib.h>  // Userland: For general utilities
    #include <math.h>    // Userland: For cabs and other math functions
    #include <complex.h> // Userland: For _Complex double type and I macro
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
 * nymya_3345_cf_swap - Applies a Controlled-Fredkin (CF-SWAP) gate to three qubits (userland).
 * @qc: Pointer to the control qubit.
 * @q1: Pointer to the first target qubit.
 * @q2: Pointer to the second target qubit.
 *
 * This function implements a Controlled-Fredkin gate. If the control qubit's
 * amplitude magnitude squared is above a threshold (representing a '1' state),
 * it performs a fermionic simulation (which includes a swap) between the two
 * target qubits. Otherwise, no action is taken on the targets.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL (invalid input).
 */
int nymya_3345_cf_swap(nymya_qubit* qc, nymya_qubit* q1, nymya_qubit* q2) {
    // Basic null pointer checks
    if (!qc || !q1 || !q2) return -1;

    // In userland, qc->amplitude is _Complex double.
    // Calculate the magnitude squared directly using cabs().
    double qc_mag_sq = cabs(qc->amplitude) * cabs(qc->amplitude);

    // Define the threshold for control qubit being '1' (0.5^2 = 0.25)
    const double threshold_sq = 0.25;

    // If the control qubit's amplitude magnitude squared is above the threshold, perform the controlled operation
    if (qc_mag_sq > threshold_sq) {
        // The userland version calls nymya_3337_fermion_sim, which includes a swap and a phase flip.
        nymya_3337_fermion_sim(q1, q2);
        log_symbolic_event("CF_SWAP", q1->id, q1->tag, "Controlled Fermionic SWAP triggered");
    } else {
        log_symbolic_event("CF_SWAP", q1->id, q1->tag, "Control=0, no action");
    }
    return 0;
}

#else // __KERNEL__

/**
 * fixed_point_magnitude_sq - Calculates the squared magnitude of a fixed-point complex number.
 * @c: The fixed-point complex number.
 *
 * Returns:
 * The squared magnitude in fixed-point format.
 */
static inline int64_t fixed_point_magnitude_sq(complex_double c) {
    // Magnitude squared = re*re + im*im
    // Note: This assumes fixed_point_mul is available via nymya.h.
    return fixed_point_mul(c.re, c.re) + fixed_point_mul(c.im, c.im);
}

/**
 * SYSCALL_DEFINE3(nymya_3345_cf_swap) - Kernel syscall for Controlled-Fredkin (CF-SWAP) gate.
 * @user_qc: User-space pointer to the control qubit structure.
 * @user_q1: User-space pointer to the first target qubit structure.
 * @user_q2: User-space pointer to the second target qubit structure.
 *
 * This syscall copies qubit data from user space to kernel space, applies the
 * Controlled-Fredkin gate logic using kernel-space functions and fixed-point
 * arithmetic, and then copies the modified data back to user space. The gate
 * conditionally applies a fermionic simulation (including a swap and phase flip)
 * to the target qubits based on the control qubit's state.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user qubit pointer is NULL.
 * - -EFAULT if copying data to/from user space fails.
 * - Error code from underlying gate operations (e.g., nymya_3337_fermion_sim).
 */
SYSCALL_DEFINE3(nymya_3345_cf_swap,
    struct nymya_qubit __user *, user_qc,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_qc, k_q1, k_q2; // Kernel-space copies
    int ret = 0; // Return value for syscall

    // 1. Check for null pointers from user-space
    if (!user_qc || !user_q1 || !user_q2) {
        pr_err("nymya_3345_cf_swap: Null user qubit pointer(s)\n");
        return -EINVAL; // Invalid argument
    }

    // 2. Copy the qubit structures from user space to kernel space
    if (copy_from_user(&k_qc, user_qc, sizeof(k_qc))) {
        pr_err("nymya_3345_cf_swap: Failed to copy k_qc from user\n");
        return -EFAULT; // Bad address
    }
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3345_cf_swap: Failed to copy k_q1 from user\n");
        return -EFAULT; // Bad address
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3345_cf_swap: Failed to copy k_q2 from user\n");
        return -EFAULT; // Bad address
    }

    // 3. Implement the control logic for kernel space
    // Define the threshold for control qubit being '1' (0.5^2 = 0.25 in floating point)
    // Convert 0.25 to fixed-point squared value
    const int64_t threshold_sq_fp = FIXED_POINT_SCALE / 4; // Represents 0.25 in Q32.32

    // Check if the control qubit's amplitude magnitude squared is above the threshold
    if (fixed_point_magnitude_sq(k_qc.amplitude) > threshold_sq_fp) {
        // Control qubit is "on" (predominantly |1>)
        // Call the kernel version of nymya_3337_fermion_sim
        ret = nymya_3337_fermion_sim(&k_q1, &k_q2);
        if (ret == 0) {
            log_symbolic_event("CF_SWAP", k_q1.id, k_q1.tag, "Controlled Fermionic SWAP triggered");
        } else {
            pr_err("nymya_3345_cf_swap: Underlying fermionic simulation failed, error %d\n", ret);
        }
    } else {
        // Control qubit is "off" (predominantly |0>)
        log_symbolic_event("CF_SWAP", k_q1.id, k_q1.tag, "Control=0, no action");
        ret = 0; // No error, just no action
    }

    // 4. Copy the modified target qubits back to user space
    // Control qubit (k_qc) is typically not modified by Fredkin/CF-SWAP,
    // so only k_q1 and k_q2 need to be copied back.
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3345_cf_swap: Failed to copy k_q1 to user\n");
        ret = -EFAULT; // Bad address
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3345_cf_swap: Failed to copy k_q2 to user\n");
        ret = -EFAULT; // Bad address
    }

    return ret; // Return 0 on success, or error code if any operation failed
}

#endif

