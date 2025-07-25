// src/nymya_3321_rotate_z.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>
#define __NR_nymya_3321_rotate_z NYMYA_ROTATE_Z_CODE

    #include <stdlib.h>
    #include <math.h>
    #include <complex.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/types.h> // For __int128 or other kernel types
    #include <linux/module.h> // ADDED: Required for EXPORT_SYMBOL_GPL
#endif

// Fixed-point constants (FIXED_POINT_PI, FIXED_POINT_PI_DIV_2) and helper functions
// (fixed_point_mul, fixed_point_cos, fixed_point_sin, fixed_sin, fixed_cos)
// are now defined as static inline in nymya.h and should NOT be redefined here.

#ifndef __KERNEL__

/**
 * nymya_3321_rotate_z - Applies a rotation around the Z axis to a qubit (userland version).
 * @q: Pointer to the qubit to be rotated.
 * @theta: Rotation angle in radians (double precision).
 *
 * Returns 0 on success, -1 if the qubit pointer is NULL.
 */
int nymya_3321_rotate_z(nymya_qubit* q, double theta) {
    if (!q) return -1;

    // Userland uses standard complex math library.
    // The complex_double type is defined as _Complex double in userspace nymya.h.
    // The complex_exp_i and complex_mul macros/functions are also defined in nymya.h
    // for userspace using standard complex operations.

    // Calculate the rotation factor e^(i * theta/2)
    complex_double rotation_factor = complex_exp_i(theta / 2.0);

    // Apply the rotation to the qubit's amplitude
    // Applying the complex multiplication: (re + i*im) * (cos + i*sin)
    // New_re = re*cos - im*sin
    // New_im = re*sin + im*cos
    q->amplitude = complex_mul(q->amplitude, rotation_factor);

    log_symbolic_event("ROT_Z", q->id, q->tag, "Applied Z-axis rotation");
    return 0;
}

#else // __KERNEL__

/**
 * nymya_3321_rotate_z - Core kernel function for Z-axis rotation on a qubit.
 * @q: Pointer to the qubit to be rotated.
 * @theta_fp: Rotation angle in fixed-point (int64_t) format.
 *
 * This function applies a rotation around the Z axis to a qubit's amplitude
 * using fixed-point arithmetic. This function is designed to be called directly
 * by other kernel code.
 *
 * Returns 0 on success, -EINVAL if the qubit pointer is NULL.
 */
int nymya_3321_rotate_z(struct nymya_qubit *q, int64_t theta_fp) {
    int64_t half_theta_fp;
    int64_t sin_half_theta_fp;
    int64_t cos_half_theta_fp;
    int64_t new_real_part;
    int64_t new_imag_part;

    if (!q) {
        pr_err("NYMYA: nymya_3321_rotate_z received NULL qubit pointer\n");
        return -EINVAL;
    }

    // Calculate half theta in fixed-point
    half_theta_fp = theta_fp >> 1; // Fixed-point division by 2

    // Compute the fixed sine and cosine for the rotation using the wrapper functions
    sin_half_theta_fp = fixed_sin(half_theta_fp);
    cos_half_theta_fp = fixed_cos(half_theta_fp);

    // Apply the Z-axis rotation to the qubit's amplitude
    // Applying the complex multiplication: (re + i*im) * (cos + i*sin)
    // New_re = re*cos - im*sin
    // New_im = re*sin + im*cos
    // All multiplications must be fixed-point multiplications using fixed_point_mul from nymya.h.
    new_real_part = fixed_point_mul(q->amplitude.re, cos_half_theta_fp) -
                    fixed_point_mul(q->amplitude.im, sin_half_theta_fp);
    new_imag_part = fixed_point_mul(q->amplitude.re, sin_half_theta_fp) +
                    fixed_point_mul(q->amplitude.im, cos_half_theta_fp);

    // Update the qubit's amplitude
    q->amplitude.re = new_real_part;
    q->amplitude.im = new_imag_part;

    log_symbolic_event("ROT_Z", q->id, q->tag, "Applied Z-axis rotation");
    return 0;
}
// Export the symbol for this function so other kernel modules/code can call it directly.
EXPORT_SYMBOL_GPL(nymya_3321_rotate_z);


/**
 * SYSCALL_DEFINE2(nymya_3321_rotate_z) - Kernel syscall for Z-axis rotation on a qubit (kernel version).
 * @user_q: User-space pointer to the qubit struct.
 * @theta_fp: Rotation angle in fixed-point (int64_t) format.
 *
 * This is the syscall entry point that wraps the core nymya_3321_rotate_z function.
 * It handles user-space copy operations before and after calling the core logic.
 *
 * Returns 0 on success, -EINVAL if user_q pointer is NULL, -EFAULT for memory access issues.
 */
SYSCALL_DEFINE2(nymya_3321_rotate_z,
    struct nymya_qubit __user *, user_q,
    int64_t, theta_fp) {

    struct nymya_qubit k_q;
    int ret;

    if (!user_q)
        return -EINVAL;

    // Copy the qubit struct from user space to kernel space
    if (copy_from_user(&k_q, user_q, sizeof(k_q)))
        return -EFAULT;

    // Call the core logic function defined above
    ret = nymya_3321_rotate_z(&k_q, theta_fp);

    if (ret) // If the core function returned an error, propagate it
        return ret;

    // Copy the modified qubit struct back to user space
    if (copy_to_user(user_q, &k_q, sizeof(k_q)))
        return -EFAULT;

    return 0;
}

#endif

