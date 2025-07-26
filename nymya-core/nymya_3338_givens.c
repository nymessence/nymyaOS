// src/nymya_3338_givens.c

#include "nymya.h" // Common definitions like complex_double

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>    // Userland only
#define __NR_nymya_3338_givens NYMYA_GIVENS_CODE

    #include <stdlib.h>  // Userland only
    #include <math.h>    // For cos, sin in userland
    #include <complex.h> // For _Complex double in userland
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h> // For -EINVAL, -EFAULT
    #include <linux/printk.h> // For pr_err
    #include <linux/module.h> // Ensure this is present for EXPORT_SYMBOL_GPL
    // No math.h or complex.h in kernel; fixed-point math assumed for amplitude operations
#endif

#ifndef __KERNEL__

/**
 * nymya_3338_givens - Applies a Givens rotation gate to two qubits (userland).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @theta: The rotation angle in radians.
 *
 * This function applies a Givens rotation, which is a two-qubit operation
 * used in quantum linear algebra. It modifies the amplitudes of the two qubits
 * based on the rotation angle.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3338_givens(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    if (!q1 || !q2) return -1;

    // In userland, q1->amplitude and q2->amplitude are _Complex double.
    _Complex double a = q1->amplitude;
    _Complex double b = q2->amplitude;

    // Apply the Givens rotation transformation
    // new_a = a * cos(theta) - b * sin(theta)
    // new_b = a * sin(theta) + b * cos(theta)
    _Complex double new_a = a * cos(theta) - b * sin(theta);
    _Complex double new_b = a * sin(theta) + b * cos(theta);

    // Assign the new complex amplitudes directly back to the qubits.
    q1->amplitude = new_a;
    q2->amplitude = new_b;

    log_symbolic_event("GIVENS", q1->id, q1->tag, "Givens rotation applied");
    return 0;
}

#else // __KERNEL__

/**
 * nymya_3338_givens - Core kernel logic for applying a Givens rotation.
 * @kq1: Pointer to the first qubit in kernel space.
 * @kq2: Pointer to the second qubit in kernel space.
 * @theta_fp: The rotation angle in fixed-point (int64_t) format.
 *
 * This function performs the Givens rotation on the provided kernel-space
 * qubit structures using fixed-point arithmetic. It modifies the amplitudes
 * of the qubits directly.
 *
 * Returns:
 * - 0 on success.
 * - (Currently, no specific error conditions are handled internally by this function,
 *    but a non-zero return value could be used for future error propagation.)
 */
int nymya_3338_givens(struct nymya_qubit *kq1, struct nymya_qubit *kq2, int64_t theta_fp) {
    // Get fixed-point cosine and sine values from nymya.h wrappers
    int64_t cos_theta_fp = fixed_cos(theta_fp);
    int64_t sin_theta_fp = fixed_sin(theta_fp);

    // Let a = kq1->amplitude and b = kq2->amplitude
    // new_a = a * cos(theta) - b * sin(theta)
    // new_b = a * sin(theta) + b * cos(theta)

    // Calculate new_a.re = a.re * cos_theta_fp - b.re * sin_theta_fp
    // Calculate new_a.im = a.im * cos_theta_fp - b.im * sin_theta_fp
    complex_double new_a_amplitude;
    new_a_amplitude.re = fixed_point_mul(kq1->amplitude.re, cos_theta_fp) -
                         fixed_point_mul(kq2->amplitude.re, sin_theta_fp);
    new_a_amplitude.im = fixed_point_mul(kq1->amplitude.im, cos_theta_fp) -
                         fixed_point_mul(kq2->amplitude.im, sin_theta_fp);

    // Calculate new_b.re = a.re * sin_theta_fp + b.re * cos_theta_fp
    // Calculate new_b.im = a.im * sin_theta_fp + b.im * cos_theta_fp
    complex_double new_b_amplitude;
    new_b_amplitude.re = fixed_point_mul(kq1->amplitude.re, sin_theta_fp) +
                         fixed_point_mul(kq2->amplitude.re, cos_theta_fp);
    new_b_amplitude.im = fixed_point_mul(kq1->amplitude.im, sin_theta_fp) +
                         fixed_point_mul(kq2->amplitude.im, cos_theta_fp);

    kq1->amplitude = new_a_amplitude;
    kq2->amplitude = new_b_amplitude;

    log_symbolic_event("GIVENS", kq1->id, kq1->tag, "Givens rotation applied");

    return 0; // Success
}
EXPORT_SYMBOL_GPL(nymya_3338_givens);



/**
 * SYSCALL_DEFINE3(nymya_3338_givens) - Kernel syscall for Givens rotation gate.
 * @user_q1: User-space pointer to the first qubit.
 * @user_q2: User-space pointer to the second qubit.
 * @theta_fp: The rotation angle in fixed-point (int64_t) format.
 *
 * This syscall copies qubit data from user space, applies the Givens rotation
 * logic using kernel-space fixed-point complex arithmetic, and then copies
 * the modified data back.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user pointer is invalid.
 * - -EFAULT if copying data to/from user space fails.
 */
SYSCALL_DEFINE3(nymya_3338_givens,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    int64_t, theta_fp) { // Theta is now fixed-point

    struct nymya_qubit k_q1, k_q2; // Kernel-space copies
    int ret; // For return value from core function

    // 1. Validate user pointers
    if (!user_q1 || !user_q2) {
        pr_err("nymya_3338_givens: Invalid user qubit pointers\n");
        return -EINVAL;
    }

    // 2. Copy qubit data from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3338_givens: Failed to copy k_q1 from user space\n");
        return -EFAULT;
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3338_givens: Failed to copy k_q2 from user space\n");
        return -EFAULT;
    }

    // 3. Call the core Givens rotation logic
    ret = nymya_3338_givens(&k_q1, &k_q2, theta_fp);
    if (ret) {
        // Propagate any error from the core logic if it ever returns one
        pr_err("nymya_3338_givens: Core logic failed with error %d\n", ret);
        return ret;
    }

    // 4. Copy modified qubit data back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3338_givens: Failed to copy k_q1 to user space\n");
        return -EFAULT;
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3338_givens: Failed to copy k_q2 to user space\n");
        return -EFAULT;
    }

    return 0; // Success
}

#endif
