// src/nymya_3327_sqrt_iswap.c

#include "nymya.h" // Includes complex_double and fixed-point scale, and now FIXED_POINT_SQRT2_INV_FP

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>
#define __NR_nymya_3327_sqrt_iswap NYMYA_SQRT_ISWAP_CODE

    #include <stdlib.h>
    #include <math.h>
    #include <complex.h> // For _Complex double and I in userland
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/slab.h> // Required for kmalloc_array if used in other syscalls
    #include <linux/types.h> // For __int128 or other kernel types
    #include <linux/module.h> // ADDED: Required for EXPORT_SYMBOL_GPL
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
 * nymya_3327_sqrt_iswap - Core kernel function for Square Root iSWAP gate.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function applies the √iSWAP gate transformation to two qubits
 * using fixed-point complex arithmetic. This function is designed to be called
 * directly by other kernel code.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any qubit pointer is NULL.
 */
int nymya_3327_sqrt_iswap(struct nymya_qubit *q1, struct nymya_qubit *q2) {
    complex_double term1;
    complex_double term2;
    complex_double scalar_factor = make_complex(FIXED_POINT_SQRT2_INV_FP, 0);

    // 1. Validate kernel qubit pointers
    if (!q1 || !q2) {
        pr_err("nymya_3327_sqrt_iswap: Invalid kernel qubit pointers\n");
        return -EINVAL;
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
    term1.re = q1->amplitude.re - q2->amplitude.im;
    term1.im = q1->amplitude.im + q2->amplitude.re;

    // Calculate (b + i * a)
    // So, i * a = -a.im + i * a.re
    // (b + i * a).re = b.re - a.im
    // (b + i * a).im = b.im + a.re
    term2.re = q2->amplitude.re - q1->amplitude.im;
    term2.im = q2->amplitude.im + q1->amplitude.re;

    // Divide by sqrt(2.0) by multiplying with 1/sqrt(2) in fixed-point
    // Use complex_mul for scalar multiplication of complex numbers
    q1->amplitude = complex_mul(term1, scalar_factor);
    q2->amplitude = complex_mul(term2, scalar_factor);

    // Log the symbolic event
    log_symbolic_event("√iSWAP", q2->id, q2->tag, "√iSWAP applied");

    return 0; // Success
}
EXPORT_SYMBOL_GPL(nymya_3327_sqrt_iswap);



// Export the symbol for this function so other kernel modules/code can call it directly.


/**
 * SYSCALL_DEFINE2(nymya_3327_sqrt_iswap) - Kernel system call implementation for √iSWAP gate.
 * @user_q1: Pointer to the first qubit structure in user space.
 * @user_q2: Pointer to the second qubit structure in user space.
 *
 * This is the syscall entry point that wraps the core nymya_3327_sqrt_iswap function.
 * It handles user-space copy operations before and after calling the core logic.
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
    int ret;

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

    // Call the core logic function defined above
    ret = nymya_3327_sqrt_iswap(&k_q1, &k_q2);

    if (ret) // If the core function returned an error, propagate it
        return ret;

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

