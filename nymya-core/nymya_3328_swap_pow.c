// src/nymya_3328_swap_pow.c

#include "nymya.h" // Includes complex_double, FIXED_POINT_SCALE

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>
#define __NR_nymya_3328_swap_pow NYMYA_SWAP_POW_CODE

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

// Define PI and PI/2 in fixed-point for kernel calculations if not already in nymya.h
// These are now defined in nymya.h and should not be redefined here.
// #ifndef FIXED_POINT_PI
// #define FIXED_POINT_PI (int64_t)(3.141592653589793 * FIXED_POINT_SCALE)
// #endif

// #ifndef FIXED_POINT_PI_DIV_2
// #define FIXED_POINT_PI_DIV_2 (int64_t)(1.5707963267948966 * FIXED_POINT_SCALE) // M_PI / 2.0
// #endif

#ifndef __KERNEL__

/**
 * nymya_3328_swap_pow - Applies an interpolated SWAP^alpha gate to two qubits (userland).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @alpha: The interpolation parameter (0 for identity, 1 for SWAP).
 *
 * This function applies a generalized SWAP gate with an interpolation parameter alpha.
 * It uses standard C complex numbers and floating-point math for calculation.
 *
 * Returns:
 * - 0 on success.
 * - -1 if either qubit pointer is NULL.
 */
int nymya_3328_swap_pow(nymya_qubit* q1, nymya_qubit* q2, double alpha) {
    if (!q1 || !q2) return -1;

    // In userland, assume nymya_qubit->amplitude is directly _Complex double.
    // No conversion from fixed-point struct is needed here.
    _Complex double a = q1->amplitude;
    _Complex double b = q2->amplitude;

    // Calculate cosine and sine components for the interpolation
    double angle = alpha * M_PI / 2.0;
    double c = cos(angle);
    double s = sin(angle);

    // Apply the interpolated SWAP gate transformation
    _Complex double new_a = c * a + s * b;
    _Complex double new_b = c * b + s * a;

    // Assign the new complex amplitudes directly back to the qubits.
    q1->amplitude = new_a;
    q2->amplitude = new_b;

    log_symbolic_event("SWAP^α", q1->id, q1->tag, "Interpolated SWAP applied");
    return 0;
}

#else // __KERNEL__

// fixed_point_mul, fixed_point_cos, fixed_point_sin are now defined as static inline in nymya.h
// and should NOT be redefined here.

/**
 * __do_sys_nymya_3328_swap_pow - Kernel system call implementation for SWAP^alpha gate.
 * @user_q1: Pointer to the first qubit structure in user space.
 * @user_q2: Pointer to the second qubit structure in user space.
 * @alpha_fp: The interpolation parameter 'alpha' in fixed-point format.
 *
 * This function copies qubit data from user space, applies the SWAP^alpha gate
 * using fixed-point complex arithmetic, and then copies the modified data back.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user pointer is invalid.
 * - -EFAULT if copying data to/from user space fails.
 */
SYSCALL_DEFINE3(nymya_3328_swap_pow,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    int64_t, alpha_fp) { // alpha is now fixed-point

    struct nymya_qubit k_q1, k_q2; // Kernel-space copies of qubits
    // int ret = 0; // Removed: unused variable

    // 1. Validate user pointers
    if (!user_q1 || !user_q2) {
        pr_err("nymya_3328_swap_pow: Invalid user qubit pointers\n");
        return -EINVAL;
    }

    // 2. Copy qubit data from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3328_swap_pow: Failed to copy k_q1 from user space\n");
        return -EFAULT;
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3328_swap_pow: Failed to copy k_q2 from user space\n");
        return -EFAULT;
    }

    // Calculate fixed-point angle: alpha * (PI / 2)
    int64_t angle_fp = fixed_point_mul(alpha_fp, FIXED_POINT_PI_DIV_2);

    // Calculate fixed-point cosine and sine values using wrappers from nymya.h
    int64_t c_fp = fixed_cos(angle_fp);
    int64_t s_fp = fixed_sin(angle_fp);

    // Get current amplitudes in fixed-point complex_double format
    complex_double a = k_q1.amplitude;
    complex_double b = k_q2.amplitude;

    // Apply the interpolated SWAP gate transformation using fixed-point complex arithmetic
    // new_a = c * a + s * b
    // new_a.re = (c.re * a.re - c.im * a.im) + (s.re * b.re - s.im * b.im)
    // new_a.im = (c.re * a.im + c.im * a.re) + (s.re * b.im + s.im * b.re)
    // Since c and s are real, c.im = 0 and s.im = 0
    // new_a.re = c_fp * a.re + s_fp * b.re (scaled)
    // new_a.im = c_fp * a.im + s_fp * b.im (scaled)
    k_q1.amplitude.re = fixed_point_mul(c_fp, a.re) + fixed_point_mul(s_fp, b.re);
    k_q1.amplitude.im = fixed_point_mul(c_fp, a.im) + fixed_point_mul(s_fp, b.im);

    // new_b = c * b + s * a
    k_q2.amplitude.re = fixed_point_mul(c_fp, b.re) + fixed_point_mul(s_fp, a.re);
    k_q2.amplitude.im = fixed_point_mul(c_fp, b.im) + fixed_point_mul(s_fp, a.im);

    // Log the symbolic event
    log_symbolic_event("SWAP^α", k_q1.id, k_q1.tag, "Interpolated SWAP applied");

    // 3. Copy modified qubit data back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3328_swap_pow: Failed to copy k_q1 to user space\n");
        return -EFAULT;
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3328_swap_pow: Failed to copy k_q2 to user space\n");
        return -EFAULT;
    }

    return 0; // Success
}

#endif

