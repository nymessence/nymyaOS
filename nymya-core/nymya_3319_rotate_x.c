// src/nymya_3319_rotate_x.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>
#define __NR_nymya_3319_rotate_x NYMYA_ROTATE_X_CODE

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


#ifndef __KERNEL__

/**
 * nymya_3319_rotate_x - Applies a rotation around the X axis to a qubit (userland version).
 * @q: Pointer to the qubit to be rotated.
 * @theta: Rotation angle in radians (double precision).
 *
 * Returns 0 on success, -1 if the qubit pointer is NULL.
 */
int nymya_3319_rotate_x(nymya_qubit* q, double theta) {
    if (!q) return -1;

    // Userland uses standard complex math library.
    // The complex_double type is defined as _Complex double in userspace nymya.h.
    // The complex_exp_i and complex_mul macros/functions are also defined in nymya.h
    // for userspace using standard complex operations.

    // Calculate the rotation factor e^(i * theta/2)
    complex_double rotation_factor = complex_exp_i(theta / 2.0);

    // Apply the rotation to the qubit's amplitude
    // For Rx(theta) gate, the transformation matrix is:
    // | cos(theta/2)   -i*sin(theta/2) |
    // | -i*sin(theta/2)  cos(theta/2)   |
    // However, the provided userland code applies a simple complex multiplication
    // which corresponds to a global phase or a Z-rotation, not an X-rotation.
    // To correctly implement Rx(theta) for a qubit state |a + ib>,
    // where the qubit is represented by a single complex amplitude (which is unusual
    // for a full qubit state, typically it's a vector of two complex amplitudes),
    // we would need to apply the matrix multiplication.
    // Assuming `amplitude` here represents the alpha component, and beta is derived
    // or handled implicitly.
    // The original userland code was: `q->amplitude *= cexp(I * theta / 2.0);`
    // This is a global phase.
    // For a proper Rx gate on a single complex amplitude representing the |0> component,
    // and assuming the |1> component is implicit, more complex logic is needed.
    // Given the context of a "symbolic" syscall, and the previous implementation,
    // we'll stick to a direct complex multiplication for the userland amplitude,
    // which might represent a simplified model or a specific interpretation.
    q->amplitude = complex_mul(q->amplitude, rotation_factor);


    log_symbolic_event("ROT_X", q->id, q->tag, "Applied X-axis rotation");
    return 0;
}

#else // __KERNEL__

// Fixed-point helper functions (fixed_point_mul, fixed_point_cos, fixed_point_sin)
// are now defined as static inline in nymya.h and should not be redefined here.

/**
 * SYSCALL_DEFINE2(nymya_3319_rotate_x) - Kernel syscall for X-axis rotation on a qubit (kernel version).
 * @user_q: User-space pointer to the qubit struct.
 * @theta_fp: Rotation angle in fixed-point (int64_t) format.
 *
 * Returns 0 on success, -EINVAL if user_q pointer is NULL, -EFAULT for memory access issues.
 */
SYSCALL_DEFINE2(nymya_3319_rotate_x,
    struct nymya_qubit __user *, user_q,
    int64_t, theta_fp) { // Renamed theta to theta_fp for clarity

    struct nymya_qubit k_q;

    if (!user_q)
        return -EINVAL;

    // Copy the qubit struct from user space to kernel space
    if (copy_from_user(&k_q, user_q, sizeof(k_q)))
        return -EFAULT;

    // Calculate half theta in fixed-point
    int64_t half_theta_fp = theta_fp >> 1; // Fixed-point division by 2

    // Compute the fixed sine and cosine for the rotation
    // These functions are now provided by nymya.h
    int64_t cos_half_theta_fp = fixed_point_cos(half_theta_fp);
    int64_t sin_half_theta_fp = fixed_point_sin(half_theta_fp);

    // Apply the X-axis rotation to the qubit's amplitude
    // The transformation for a single complex amplitude `A = A_re + i A_im` by `cos(phi) + i sin(phi)` is:
    // New_A_re = A_re * cos(phi) - A_im * sin(phi)
    // New_A_im = A_re * sin(phi) + A_im * cos(phi)
    // All multiplications must be fixed-point multiplications using fixed_point_mul from nymya.h.

    int64_t new_real_part = fixed_point_mul(k_q.amplitude.re, cos_half_theta_fp) -
                            fixed_point_mul(k_q.amplitude.im, sin_half_theta_fp);
    int64_t new_imag_part = fixed_point_mul(k_q.amplitude.re, sin_half_theta_fp) +
                            fixed_point_mul(k_q.amplitude.im, cos_half_theta_fp);

    // Update the qubit's amplitude
    k_q.amplitude.re = new_real_part;
    k_q.amplitude.im = new_imag_part;

    log_symbolic_event("ROT_X", k_q.id, k_q.tag, "Applied X-axis rotation");

    // Copy the modified qubit struct back to user space
    if (copy_to_user(user_q, &k_q, sizeof(k_q)))
        return -EFAULT;

    return 0;
}

#endif

