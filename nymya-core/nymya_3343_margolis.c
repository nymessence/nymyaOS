// src/nymya_3343_margolis.c
//
// This file implements the nymya_3343_margolis syscall, which applies a
// three-qubit Margolis gate. The Margolis gate is a controlled operation
// where a phase flip is applied to the target qubit if both control qubits
// are in a specific state (typically |1>).
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
 * nymya_3343_margolis - Applies a Margolis gate to three qubits (userland).
 * @qc1: Pointer to the first control qubit.
 * @qc2: Pointer to the second control qubit.
 * @qt: Pointer to the target qubit.
 *
 * This function implements a Margolis gate. If both control qubits' amplitude
 * magnitudes squared are above a threshold (representing a '1' state), it
 * applies a phase flip (multiplies by -1) to the target qubit's amplitude.
 * Otherwise, no action is taken.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL (invalid input).
 */
int nymya_3343_margolis(nymya_qubit* qc1, nymya_qubit* qc2, nymya_qubit* qt) {
    // Basic null pointer checks
    if (!qc1 || !qc2 || !qt) return -1;

    // In userland, amplitudes are _Complex double.
    // Calculate magnitude squared for each control qubit.
    double qc1_mag_sq = cabs(qc1->amplitude) * cabs(qc1->amplitude);
    double qc2_mag_sq = cabs(qc2->amplitude) * cabs(qc2->amplitude);

    // Define the threshold for control qubit being '1' (0.5^2 = 0.25)
    const double threshold_sq = 0.25;

    // If both control qubits' amplitude magnitudes squared are above the threshold, apply phase flip
    if (qc1_mag_sq > threshold_sq && qc2_mag_sq > threshold_sq) {
        qt->amplitude *= -1; // Apply phase flip (multiply by -1)
        log_symbolic_event("MARGOLIS", qt->id, qt->tag, "Margolis gate triggered");
    } else {
        log_symbolic_event("MARGOLIS", qt->id, qt->tag, "Conditions not met");
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
 * SYSCALL_DEFINE3(nymya_3343_margolis) - Kernel syscall for Margolis gate.
 * @user_qc1: User-space pointer to the first control qubit structure.
 * @user_qc2: User-space pointer to the second control qubit structure.
 * @user_qt: User-space pointer to the target qubit structure.
 *
 * This syscall copies qubit data from user space to kernel space, applies the
 * Margolis gate logic using kernel-space fixed-point arithmetic, and then
 * copies the modified data back to user space. The gate applies a phase flip
 * to the target qubit if both control qubits are in a '1' state (based on amplitude magnitude).
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user qubit pointer is NULL.
 * - -EFAULT if copying data to/from user space fails.
 */
SYSCALL_DEFINE3(nymya_3343_margolis,
    struct nymya_qubit __user *, user_qc1,
    struct nymya_qubit __user *, user_qc2,
    struct nymya_qubit __user *, user_qt) {

    struct nymya_qubit k_qc1, k_qc2, k_qt; // Kernel-space copies
    int ret = 0; // Return value for syscall

    // 1. Check for null pointers from user-space
    if (!user_qc1 || !user_qc2 || !user_qt) {
        pr_err("nymya_3343_margolis: Null user qubit pointer(s)\n");
        return -EINVAL; // Invalid argument
    }

    // 2. Copy the qubit structures from user space to kernel space
    if (copy_from_user(&k_qc1, user_qc1, sizeof(k_qc1))) {
        pr_err("nymya_3343_margolis: Failed to copy k_qc1 from user\n");
        return -EFAULT; // Bad address
    }
    if (copy_from_user(&k_qc2, user_qc2, sizeof(k_qc2))) {
        pr_err("nymya_3343_margolis: Failed to copy k_qc2 from user\n");
        return -EFAULT; // Bad address
    }
    if (copy_from_user(&k_qt, user_qt, sizeof(k_qt))) {
        pr_err("nymya_3343_margolis: Failed to copy k_qt from user\n");
        return -EFAULT; // Bad address
    }

    // 3. Implement the Margolis gate logic for kernel space
    // Define the threshold for control qubit being '1' (0.5^2 = 0.25 in floating point)
    // Convert 0.25 to fixed-point squared value
    const int64_t threshold_sq_fp = FIXED_POINT_SCALE / 4; // Represents 0.25 in Q32.32

    // Check if both control qubits' amplitude magnitudes squared are above the threshold
    if (fixed_point_magnitude_sq(k_qc1.amplitude) > threshold_sq_fp &&
        fixed_point_magnitude_sq(k_qc2.amplitude) > threshold_sq_fp) {
        // Apply phase flip (multiply by -1) to the target qubit's amplitude
        // For a complex number (re + i*im), multiplying by -1 results in (-re - i*im).
        k_qt.amplitude.re = -k_qt.amplitude.re;
        k_qt.amplitude.im = -k_qt.amplitude.im;

        log_symbolic_event("MARGOLIS", k_qt.id, k_qt.tag, "Margolis gate triggered");
        ret = 0; // Indicate success for this path
    } else {
        log_symbolic_event("MARGOLIS", k_qt.id, k_qt.tag, "Conditions not met");
        ret = 0; // Indicate success even if conditions not met (no change)
    }

    // 4. Copy the modified target qubit back to user space
    // Control qubits (k_qc1, k_qc2) are typically not modified by Margolis gate,
    // so only k_qt needs to be copied back.
    if (copy_to_user(user_qt, &k_qt, sizeof(k_qt))) {
        pr_err("nymya_3343_margolis: Failed to copy k_qt to user\n");
        ret = -EFAULT; // Bad address
    }

    return ret; // Return 0 on success, or error code if copy_to_user failed
}

#endif

