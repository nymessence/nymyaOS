// src/nymya_3321_rotate_z.c

#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <complex.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/types.h> // For __int128 or other kernel types
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
    // The original userland code was: `q->amplitude *= cos(theta / 2) + I * sin(theta / 2);`
    // This is a global phase. For a proper Rz gate on a single complex amplitude
    // representing the |0> component, and assuming the |1> component is implicit,
    // more complex logic is needed. Given the context of a "symbolic" syscall,
    // and the previous implementation, we'll stick to a direct complex multiplication
    // for the userland amplitude, which might represent a simplified model or a specific interpretation.
    q->amplitude = complex_mul(q->amplitude, rotation_factor);

    log_symbolic_event("ROT_Z", q->id, q->tag, "Applied Z-axis rotation");
    return 0;
}

#else // __KERNEL__

// Fixed-point helper functions (fixed_point_mul, fixed_point_cos, fixed_point_sin)
// are now defined as static inline in nymya.h and should not be redefined here.
// We will use the fixed_sin and fixed_cos wrappers.

/**
 * SYSCALL_DEFINE2(nymya_3321_rotate_z) - Kernel syscall for Z-axis rotation on a qubit (kernel version).
 * @user_q: User-space pointer to the qubit struct.
 * @theta_fp: Rotation angle in fixed-point (int64_t) format.
 *
 * Returns 0 on success, -EINVAL if user_q pointer is NULL, -EFAULT for memory access issues.
 */
SYSCALL_DEFINE2(nymya_3321_rotate_z,
    struct nymya_qubit __user *, user_q,
    int64_t, theta_fp) {

    struct nymya_qubit k_q;

    if (!user_q)
        return -EINVAL;

    // Copy the qubit struct from user space to kernel space
    if (copy_from_user(&k_q, user_q, sizeof(k_q)))
        return -EFAULT;

    // Calculate half theta in fixed-point
    int64_t half_theta_fp = theta_fp >> 1; // Fixed-point division by 2

    // Compute the fixed sine and cosine for the rotation using the wrapper functions
    int64_t sin_half_theta_fp = fixed_sin(half_theta_fp);
    int64_t cos_half_theta_fp = fixed_cos(half_theta_fp);

    // Apply the Z-axis rotation to the qubit's amplitude
    // Applying the complex multiplication: (re + i*im) * (cos + i*sin)
    // New_re = re*cos - im*sin
    // New_im = re*sin + im*cos
    // All multiplications must be fixed-point multiplications using fixed_point_mul from nymya.h.
    int64_t new_real_part = fixed_point_mul(k_q.amplitude.re, cos_half_theta_fp) -
                            fixed_point_mul(k_q.amplitude.im, sin_half_theta_fp);
    int64_t new_imag_part = fixed_point_mul(k_q.amplitude.re, sin_half_theta_fp) +
                            fixed_point_mul(k_q.amplitude.im, cos_half_theta_fp);

    // Update the qubit's amplitude
    k_q.amplitude.re = new_real_part;
    k_q.amplitude.im = new_imag_part;

    log_symbolic_event("ROT_Z", k_q.id, k_q.tag, "Applied Z-axis rotation");

    // Copy the modified qubit struct back to user space
    if (copy_to_user(user_q, &k_q, sizeof(k_q)))
        return -EFAULT;

    return 0;
}

#endif

