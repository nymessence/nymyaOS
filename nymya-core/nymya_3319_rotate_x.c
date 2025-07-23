// src/nymya_3319_rotate_x.c

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

    // Apply the X-axis rotation (e^(i * theta / 2)) using standard double precision math
    q->amplitude *= cos(theta / 2) + I * sin(theta / 2);

    log_symbolic_event("ROT_X", q->id, q->tag, "Applied X-axis rotation");
    return 0;
}

#else

/**
 * SYSCALL_DEFINE2(nymya_3319_rotate_x) - Kernel syscall for X-axis rotation on a qubit (kernel version).
 * @user_q: User-space pointer to the qubit struct.
 * @theta: Rotation angle in radians (fixed-point, Q32.32 format).
 *
 * Returns 0 on success, -EINVAL if user_q pointer is NULL, -EFAULT for memory access issues.
 */
SYSCALL_DEFINE2(nymya_3319_rotate_x,
    struct nymya_qubit __user *, user_q,
    int64_t, theta) {

    struct nymya_qubit k_q;

    if (!user_q)
        return -EINVAL;

    // Copy the qubit struct from user space to kernel space
    if (copy_from_user(&k_q, user_q, sizeof(k_q)))
        return -EFAULT;

    // Compute the fixed sine and cosine for the rotation (using the already defined fixed-point functions)
    int64_t sin_val = fixed_sin(theta / 2);  // Imaginary part (sin(theta / 2))
    int64_t cos_val = fixed_cos(theta / 2);  // Real part (cos(theta / 2))

    // Apply the X-axis rotation to the qubit's amplitude
    // Amplitude is a complex number represented by a fixed-point pair (real, imaginary)
    // Apply the rotation: amplitude *= cos_val + sin_val * i
    // In kernel space, we must multiply and update both real and imaginary components.

    int64_t real_part = k_q.amplitude.re * cos_val - k_q.amplitude.im * sin_val;
    int64_t imag_part = k_q.amplitude.re * sin_val + k_q.amplitude.im * cos_val;

    // Update the qubit's amplitude
    k_q.amplitude.re = real_part;
    k_q.amplitude.im = imag_part;

    log_symbolic_event("ROT_X", k_q.id, k_q.tag, "Applied X-axis rotation");

    // Copy the modified qubit struct back to user space
    if (copy_to_user(user_q, &k_q, sizeof(k_q)))
        return -EFAULT;

    return 0;
}

#endif
