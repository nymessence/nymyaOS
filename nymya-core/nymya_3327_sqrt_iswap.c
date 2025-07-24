// src/nymya_3327_sqrt_iswap.c

#include "nymya.h" // Includes complex_double and fixed-point scale, and now FIXED_POINT_SQRT2_INV_FP

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <complex.h> // For _Complex double and I in userland
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/slab.h> // Required for kmalloc_array if used in other syscalls
    #include <linux/types.h> // For __int128 or other kernel types
    // No math.h or complex.h in kernel
#endif

#ifndef __KERNEL__

/**
 * nymya_3327_sqrt_iswap - Applies the square root of iSWAP gate to two qubits (userland).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function applies the √iSWAP gate, which entangles two qubits.
 * It uses standard C complex numbers for calculation.
 *
 * Returns:
 * - 0 on success.
 * - -1 if either qubit pointer is NULL.
 */
int nymya_3327_sqrt_iswap(nymya_qubit* q1, nymya_qubit* q2) {
    if (!q1 || !q2) return -1;

    // In userland, assume nymya_qubit->amplitude is directly _Complex double.
    // No conversion from fixed-point struct is needed here.
    _Complex double a = q1->amplitude;
    _Complex double b = q2->amplitude;

    // Apply the √iSWAP gate transformation
    _Complex double new_a = (a + I * b) / sqrt(2.0);
    _Complex double new_b = (b + I * a) / sqrt(2.0);

    // Assign the new complex amplitudes directly back to the qubits.
    q1->amplitude = new_a;
    q2->amplitude = new_b;

    log_symbolic_event("√iSWAP", q2->id, q2->tag, "√iSWAP applied");
    return 0;
}

#else // __KERNEL__

/**
 * fixed_point_mul - Performs fixed-point multiplication.
 * @val1: The first fixed-point value.
 * @val2: The second fixed-point value.
 *
 * Multiplies two fixed-point numbers and scales the result back to fixed-point.
 * Uses __int128 for intermediate calculation to prevent overflow.
 *
 * Returns:
 * The product as an int64_t (fixed-point).
 */
static inline int64_t fixed_point_mul(int64_t val1, int64_t val2) {
    // Corrected to use __int128 (two underscores) as a GCC extension
    return (int64_t)(((__int128)val1 * val2) >> 32);
}

/**
 * __do_sys_nymya_3327_sqrt_iswap - Kernel system call implementation for √iSWAP gate.
 * @user_q1: Pointer to the first qubit structure in user space.
 * @user_q2: Pointer to the second qubit structure in user space.
 *
 * This function copies qubit data from user space, applies the √iSWAP gate
 * using fixed-point complex arithmetic, and then copies the modified data back.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user pointer is invalid.
 * - -EFAULT if copying data to/from user space fails.
 */
SYSCALL_DEFINE2(nymya_3327_sqrt_iswap,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2; // Kernel-space copies of qubits
    // int ret = 0; // Removed: unused variable

    // 1. Validate user pointers
    if (!user_q1 || !user_q2) {
        pr_err("nymya_3327_sqrt_iswap: Invalid user qubit pointers\n");
        return -EINVAL;
    }

    // 2. Copy qubit data from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3327_sqrt_iswap: Failed to copy k_q1 from user space\n");
        return -EFAULT;
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3327_sqrt_iswap: Failed to copy k_q2 from user space\n");
        return -EFAULT;
    }

    // Perform √iSWAP gate transformation using fixed-point arithmetic
    // New amplitudes:
    // q1_new = (a + i * b) / sqrt(2.0)
    // q2_new = (b + i * a) / sqrt(2.0)

    // Calculate (a + i * b)
    // If X = X_re + iX_im, then iX = -X_im + iX_re
    // So, i * b = -b.im + i * b.re
    // (a + i * b).re = a.re - b.im
    // (a + i * b).im = a.im + b.re
    complex_double term1;
    term1.re = k_q1.amplitude.re - k_q2.amplitude.im;
    term1.im = k_q1.amplitude.im + k_q2.amplitude.re;

    // Calculate (b + i * a)
    // So, i * a = -a.im + i * a.re
    // (b + i * a).re = b.re - a.im
    // (b + i * a).im = b.im + a.re
    complex_double term2;
    term2.re = k_q2.amplitude.re - k_q1.amplitude.im;
    term2.im = k_q2.amplitude.im + k_q1.amplitude.re;

    // Divide by sqrt(2.0) by multiplying with 1/sqrt(2) in fixed-point
    k_q1.amplitude.re = fixed_point_mul(term1.re, FIXED_POINT_SQRT2_INV_FP);
    k_q1.amplitude.im = fixed_point_mul(term1.im, FIXED_POINT_SQRT2_INV_FP);
    k_q2.amplitude.re = fixed_point_mul(term2.re, FIXED_POINT_SQRT2_INV_FP);
    k_q2.amplitude.im = fixed_point_mul(term2.im, FIXED_POINT_SQRT2_INV_FP);

    // Log the symbolic event
    log_symbolic_event("√iSWAP", k_q2.id, k_q2.tag, "√iSWAP applied");

    // 3. Copy modified qubit data back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3327_sqrt_iswap: Failed to copy k_q1 to user space\n");
        return -EFAULT;
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3327_sqrt_iswap: Failed to copy k_q2 to user space\n");
        return -EFAULT;
    }

    return 0; // Success
}

#endif

