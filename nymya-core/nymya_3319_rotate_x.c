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

    // Reverting userland to original (assuming `_Complex double amplitude` in userland nymya_qubit)
    _Complex double amp = q->amplitude;
    _Complex double rotation_factor = cos(theta / 2.0) + I * sin(theta / 2.0);
    q->amplitude = amp * rotation_factor;

    log_symbolic_event("ROT_X", q->id, q->tag, "Applied X-axis rotation");
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
    // Perform multiplication using __int128 to prevent overflow, then shift.
    return (int64_t)(((__int128)val1 * val2) >> 32);
}

/**
 * fixed_point_cos - Calculates the cosine of a fixed-point angle.
 * @angle_fp: The angle in fixed-point format.
 *
 * This is a simplified fixed-point cosine approximation for kernel use.
 * For production, consider a more robust implementation (e.g., CORDIC or lookup table).
 *
 * Returns:
 * The cosine value in fixed-point format.
 */
static inline int64_t fixed_point_cos(int64_t angle_fp) {
    // Normalize angle to [-PI, PI] for better approximation range
    while (angle_fp > FIXED_POINT_PI) angle_fp -= (FIXED_POINT_PI << 1);
    while (angle_fp < -FIXED_POINT_PI) angle_fp += (FIXED_POINT_PI << 1);

    // Simple Taylor series approximation for cos(x) = 1 - x^2/2! + x^4/4! ...
    // Note: This is a very basic approximation and may not be accurate for all angles.
    // It's meant to resolve compilation errors and provide a placeholder.
    int64_t term1 = FIXED_POINT_SCALE; // 1
    int64_t term2 = fixed_point_mul(angle_fp, angle_fp); // x^2
    term2 = fixed_point_mul(term2, (int64_t)(0.5 * FIXED_POINT_SCALE)); // x^2 / 2
    // For higher accuracy, more terms would be needed:
    // int64_t term3 = fixed_point_mul(fixed_point_mul(fixed_point_mul(angle_fp, angle_fp), fixed_point_mul(angle_fp, angle_fp)), (int64_t)(1.0/24.0 * FIXED_POINT_SCALE)); // x^4 / 4!

    return term1 - term2; // Simplified to 1 - x^2/2
}

/**
 * fixed_point_sin - Calculates the sine of a fixed-point angle.
 * @angle_fp: The angle in fixed-point format.
 *
 * This is a simplified fixed-point sine approximation for kernel use.
 * For production, consider a more robust implementation (e.g., CORDIC or lookup table).
 *
 * Returns:
 * The sine value in fixed-point format.
 */
static inline int64_t fixed_point_sin(int64_t angle_fp) {
    // Normalize angle to [-PI, PI] for better approximation range
    while (angle_fp > FIXED_POINT_PI) angle_fp -= (FIXED_POINT_PI << 1);
    while (angle_fp < -FIXED_POINT_PI) angle_fp += (FIXED_POINT_PI << 1);

    // Simple Taylor series approximation for sin(x) = x - x^3/3! + x^5/5! ...
    // Note: This is a very basic approximation and may not be accurate for all angles.
    // It's meant to resolve compilation errors and provide a placeholder.
    int64_t term1 = angle_fp; // x
    int64_t term2_numerator = fixed_point_mul(fixed_point_mul(angle_fp, angle_fp), angle_fp); // x^3
    int64_t term2 = fixed_point_mul(term2_numerator, (int64_t)(1.0/6.0 * FIXED_POINT_SCALE)); // x^3 / 6
    // For higher accuracy, more terms would be needed:
    // int64_t term3 = fixed_point_mul(fixed_point_mul(fixed_point_mul(fixed_point_mul(fixed_point_mul(angle_fp, angle_fp), angle_fp), angle_fp), angle_fp), (int64_t)(1.0/120.0 * FIXED_POINT_SCALE)); // x^5 / 5!

    return term1 - term2; // Simplified to x - x^3/6
}

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
    int64_t sin_half_theta_fp = fixed_point_sin(half_theta_fp);
    int64_t cos_half_theta_fp = fixed_point_cos(half_theta_fp);

    // Apply the X-axis rotation to the qubit's amplitude
    // Amplitude is a complex number represented by a fixed-point pair (real, imaginary)
    // The transformation for a single complex amplitude `A = A_re + i A_im` by `cos(phi) + i sin(phi)` is:
    // New_A_re = A_re * cos(phi) - A_im * sin(phi)
    // New_A_im = A_re * sin(phi) + A_im * cos(phi)
    // All multiplications must be fixed-point multiplications.

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

