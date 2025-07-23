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
#endif

/**
 * nymya_3321_rotate_z - Applies a rotation around the Z axis to a qubit (userland version).
 * 
 * This function applies a simplified rotation of the qubit around the Z axis in the Bloch
 * sphere. The operation used is a mathematical approximation where the qubit's amplitude is
 * multiplied by e^(i * theta / 2). This is commonly used for rotations around the Z axis in 
 * quantum mechanics.
 * 
 * The function logs the event symbolically for traceability.
 * 
 * @q: Pointer to the qubit to be rotated (nymya_qubit struct).
 * @theta: Rotation angle in radians (angle by which to rotate the qubit).
 * 
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL (invalid input).
 */
#ifndef __KERNEL__

int nymya_3321_rotate_z(nymya_qubit* q, double theta) {
    // Check if the qubit pointer is NULL
    if (!q) return -1;

    // Apply the Z-axis rotation (e^(i * theta / 2)) approximation
    q->amplitude *= cos(theta / 2.0) + I * sin(theta / 2.0);

    // Log the symbolic event for the rotation applied
    log_symbolic_event("ROT_Z", q->id, q->tag, "Applied Z-axis rotation");
    return 0;
}

#else

/**
 * SYSCALL_DEFINE2(nymya_3321_rotate_z) - Kernel syscall for Z-axis rotation on a qubit.
 * 
 * This kernel syscall provides functionality to rotate the amplitude of a qubit 
 * around the Z axis. It copies the qubit from user space, applies the Z-axis 
 * rotation by multiplying the amplitude by e^(i * theta / 2), logs the symbolic 
 * event for the rotation, and then copies the modified qubit back to user space.
 * 
 * The rotation approximation uses fixed-point arithmetic to handle the trigonometric 
 * functions for better precision and efficiency within the kernel.
 * 
 * @user_q: User-space pointer to the qubit struct.
 * @theta: Rotation angle in radians (angle by which to rotate the qubit, passed in fixed-point format).
 * 
 * Returns:
 * - 0 on success.
 * - -EINVAL if the user_q pointer is NULL (invalid input).
 * - -EFAULT if copy_from_user or copy_to_user fails (memory access issues).
 */
SYSCALL_DEFINE2(nymya_3321_rotate_z,
    struct nymya_qubit __user *, user_q,
    int64_t, theta) {

    struct nymya_qubit k_q;

    // Validate the user pointer
    if (!user_q)
        return -EINVAL;

    // Copy the qubit struct from user space to kernel space
    if (copy_from_user(&k_q, user_q, sizeof(k_q)))
        return -EFAULT;

    // Compute the fixed-point sine and cosine for the rotation (using the already defined fixed-point functions)
    int64_t sin_val = fixed_sin(theta / 2);  // Imaginary part (sin(theta / 2))
    int64_t cos_val = fixed_cos(theta / 2);  // Real part (cos(theta / 2))

    // Apply the Z-axis rotation to the qubit's amplitude (in fixed-point)
    int64_t real_part = k_q.amplitude.re * cos_val - k_q.amplitude.im * sin_val;
    int64_t imag_part = k_q.amplitude.re * sin_val + k_q.amplitude.im * cos_val;

    // Update the qubit's amplitude
    k_q.amplitude.re = real_part;
    k_q.amplitude.im = imag_part;

    // Log the symbolic event for the rotation applied
    log_symbolic_event("ROT_Z", k_q.id, k_q.tag, "Applied Z-axis rotation");

    // Copy the modified qubit struct back to user space
    if (copy_to_user(user_q, &k_q, sizeof(k_q)))
        return -EFAULT;

    return 0;
}

#endif

