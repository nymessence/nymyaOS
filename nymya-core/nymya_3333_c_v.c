// src/nymya_3333_c_v.c

#include "nymya.h" // Common definitions like complex_double

#ifndef __KERNEL__
    #include <stdio.h>    // Userland only
    #include <stdlib.h>  // Userland only
    #include <math.h>    // For cabs in userland
    #include <complex.h> // For _Complex double in userland
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h> // For -EINVAL, -EFAULT
    // No math.h or complex.h in kernel; fixed-point math assumed for amplitude operations
#endif

#ifndef __KERNEL__

// Userland implementation
int nymya_3333_c_v(nymya_qubit* qc, nymya_qubit* qt) {
    if (!qc || !qt) return -1;

    // In userland, cabs calculates the magnitude of a complex double.
    // The condition `cabs(qc->amplitude) > 0.5` implies that the control qubit
    // is predominantly in the |1> state (amplitude magnitude squared > 0.25).
    // This is a simplified check for a "controlled" operation.
    if (cabs(qc->amplitude) > 0.5) {
        nymya_3307_sqrt_x_gate(qt); // Apply sqrt(X) gate to target qubit
        log_symbolic_event("C_V", qt->id, qt->tag, "Controlled-V applied");
    } else {
        log_symbolic_event("C_V", qt->id, qt->tag, "Control=0, no action");
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
    // Use fixed_point_mul for each term to correctly handle scaling.
    return fixed_point_mul(c.re, c.re) + fixed_point_mul(c.im, c.im);
}

/**
 * SYSCALL_DEFINE2(nymya_3333_c_v) - Kernel syscall for Controlled-V gate.
 * @user_qc: User-space pointer to the control qubit.
 * @user_qt: User-space pointer to the target qubit.
 *
 * This syscall copies qubit data from user space, applies the Controlled-V gate
 * logic using fixed-point arithmetic, and then copies the modified data back.
 * The V gate is typically defined as sqrt(X).
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user pointer is invalid.
 * - -EFAULT if copying data to/from user space fails.
 * - Error code from underlying gate operations (e.g., nymya_3307_sqrt_x_gate).
 */
SYSCALL_DEFINE2(nymya_3333_c_v,
    struct nymya_qubit __user *, user_qc,
    struct nymya_qubit __user *, user_qt) {

    struct nymya_qubit k_qc, k_qt; // Kernel-space copies
    int ret = 0;

    // 1. Validate user pointers
    if (!user_qc || !user_qt) {
        pr_err("nymya_3333_c_v: Invalid user qubit pointers\n");
        return -EINVAL;
    }

    // 2. Copy qubit data from user space to kernel space
    if (copy_from_user(&k_qc, user_qc, sizeof(k_qc))) {
        pr_err("nymya_3333_c_v: Failed to copy k_qc from user space\n");
        return -EFAULT;
    }
    if (copy_from_user(&k_qt, user_qt, sizeof(k_qt))) {
        pr_err("nymya_3333_c_v: Failed to copy k_qt from user space\n");
        return -EFAULT;
    }

    // 3. Implement the control logic for kernel space
    // For a control qubit, we check if its amplitude squared magnitude is above a threshold.
    // A common threshold for |1> state is when |amplitude|^2 is close to 1.
    // Here, 0.5 * FIXED_POINT_SCALE is equivalent to a magnitude of 0.5 (0.5^2 = 0.25).
    // If we want to check for the |1> state, the amplitude of |1> is 1, so its magnitude squared is 1.
    // A simple threshold like 0.75 * FIXED_POINT_SCALE (for magnitude squared) might be more appropriate
    // if the control is truly meant to be in a definite |1> state.
    // Let's use a threshold for the *squared* magnitude, e.g., 0.5 (which is 0.25 in magnitude).
    // So, 0.5 * FIXED_POINT_SCALE is 0.5 in Q32.32, meaning 0.5 * (2^32).
    // The comparison `cabs(qc->amplitude) > 0.5` in userland means `sqrt(re*re + im*im) > 0.5`,
    // which is `re*re + im*im > 0.25`.
    // In fixed-point, 0.25 * FIXED_POINT_SCALE is `(1ULL << 32) / 4`.
    // Let's use a threshold for the squared magnitude in fixed-point.
    int64_t threshold_sq_fp = FIXED_POINT_SCALE / 4; // Represents 0.25 in Q32.32

    if (fixed_point_magnitude_sq(k_qc.amplitude) > threshold_sq_fp) {
        // Control qubit is "on" (predominantly |1>)
        // Call the kernel version of nymya_3307_sqrt_x_gate
        ret = nymya_3307_sqrt_x_gate(&k_qt);
        if (ret == 0) {
            log_symbolic_event("C_V", k_qt.id, k_qt.tag, "Controlled-V applied");
        } else {
            pr_err("nymya_3333_c_v: Failed to apply sqrt_x_gate to target qubit, error %d\n", ret);
        }
    } else {
        // Control qubit is "off" (predominantly |0>)
        log_symbolic_event("C_V", k_qt.id, k_qt.tag, "Control=0, no action");
    }

    // 4. Copy modified target qubit data back to user space
    if (copy_to_user(user_qt, &k_qt, sizeof(k_qt))) {
        pr_err("nymya_3333_c_v: Failed to copy k_qt to user space\n");
        return -EFAULT;
    }

    return ret; // Return the result of the operation (0 for success, error code otherwise)
}

#endif

