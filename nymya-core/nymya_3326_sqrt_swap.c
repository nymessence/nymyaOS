// src/nymya_3326_sqrt_swap.c

#include "nymya.h" // Includes complex_double and fixed-point scale

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>
#define __NR_nymya_3326_sqrt_swap NYMYA_SQRT_SWAP_CODE

    #include <stdlib.h>
    #include <math.h>
    #include <complex.h> // For _Complex double and I in userland
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/slab.h> // Required for kmalloc_array if used in other syscalls
    #include <linux/types.h> // For __int128 or other kernel types
    #include <linux/module.h> // Required for EXPORT_SYMBOL_GPL
    // No math.h or complex.h in kernel
#endif

#ifndef __KERNEL__

/**
 * nymya_3326_sqrt_swap - Applies the square root of SWAP gate to two qubits (userland).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function applies the √SWAP gate, which entangles two qubits.
 * It uses standard C complex numbers for calculation.
 *
 * Returns:
 * - 0 on success.
 * - -1 if either qubit pointer is NULL.
 */
int nymya_3326_sqrt_swap(nymya_qubit* q1, nymya_qubit* q2) {
    if (!q1 || !q2) return -1;

    // In userland, assume nymya_qubit->amplitude is directly _Complex double.
    // No conversion from fixed-point struct is needed here.
    _Complex double a = q1->amplitude;
    _Complex double b = q2->amplitude;

    // Apply the √SWAP gate transformation
    _Complex double new_a = 0.5 * (a + b + I * (a - b));
    _Complex double new_b = 0.5 * (a + b - I * (a - b));

    // Assign the new complex amplitudes directly back to the qubits.
    q1->amplitude = new_a;
    q2->amplitude = new_b;

    log_symbolic_event("SQRT_SWAP", q1->id, q1->tag, "√SWAP applied");
    return 0;
}

#else // __KERNEL__

/**
 * nymya_3326_sqrt_swap - Applies the square root of SWAP gate to two qubits (kernel-side).
 * @kq1: Pointer to the first qubit in kernel space.
 * @kq2: Pointer to the second qubit in kernel space.
 *
 * This function applies the √SWAP gate using fixed-point complex arithmetic.
 * It operates directly on kernel-space qubit structures.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any kernel qubit pointer is NULL.
 */
int nymya_3326_sqrt_swap(struct nymya_qubit *kq1, struct nymya_qubit *kq2) {
    if (!kq1 || !kq2) {
        pr_err("nymya_3326_sqrt_swap (core): Invalid kernel qubit pointers\n");
        return -EINVAL; // Should not happen if called correctly from SYSCALL, but good for robustness
    }

    // Perform √SWAP gate transformation using fixed-point arithmetic
    // Let a = kq1->amplitude and b = kq2->amplitude
    // New amplitudes:
    // q1_new = 0.5 * (a + b + i * (a - b))
    // q2_new = 0.5 * (a + b - i * (a - b))

    // Calculate (a + b)
    complex_double sum_amplitude;
    sum_amplitude.re = kq1->amplitude.re + kq2->amplitude.re;
    sum_amplitude.im = kq1->amplitude.im + kq2->amplitude.im;

    // Calculate (a - b)
    complex_double diff_amplitude;
    diff_amplitude.re = kq1->amplitude.re - kq2->amplitude.re;
    diff_amplitude.im = kq1->amplitude.im - kq2->amplitude.im;

    // Calculate i * (a - b)
    // If X = X_re + iX_im, then iX = -X_im + iX_re
    complex_double i_times_diff;
    i_times_diff.re = -diff_amplitude.im;
    i_times_diff.im = diff_amplitude.re;

    // Calculate (a + b + i * (a - b)) and divide by 2
    kq1->amplitude.re = (sum_amplitude.re + i_times_diff.re) >> 1;
    kq1->amplitude.im = (sum_amplitude.im + i_times_diff.im) >> 1;

    // Calculate (a + b - i * (a - b)) and divide by 2
    kq2->amplitude.re = (sum_amplitude.re - i_times_diff.re) >> 1;
    kq2->amplitude.im = (sum_amplitude.im - i_times_diff.im) >> 1;

    // Log the symbolic event
    log_symbolic_event("SQRT_SWAP", kq1->id, kq1->tag, "√SWAP applied");

    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3326_sqrt_swap);



/**
 * __do_sys_nymya_3326_sqrt_swap - Kernel system call implementation for √SWAP gate.
 * @user_q1: Pointer to the first qubit structure in user space.
 * @user_q2: Pointer to the second qubit structure in user space.
 *
 * This function copies qubit data from user space, applies the √SWAP gate
 * using fixed-point complex arithmetic, and then copies the modified data back.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user pointer is invalid.
 * - -EFAULT if copying data to/from user space fails.
 */
SYSCALL_DEFINE2(nymya_3326_sqrt_swap,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2; // Kernel-space copies of qubits
    int ret = 0;

    // 1. Validate user pointers
    if (!user_q1 || !user_q2) {
        pr_err("nymya_3326_sqrt_swap: Invalid user qubit pointers\n");
        return -EINVAL;
    }

    // 2. Copy qubit data from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3326_sqrt_swap: Failed to copy k_q1 from user space\n");
        return -EFAULT;
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3326_sqrt_swap: Failed to copy k_q2 from user space\n");
        return -EFAULT;
    }

    // Call the core logic function
    ret = nymya_3326_sqrt_swap(&k_q1, &k_q2);
    if (ret) {
        pr_err("nymya_3326_sqrt_swap: Core logic failed with error %d\n", ret);
        return ret; // Propagate error from core function
    }

    // 3. Copy modified qubit data back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3326_sqrt_swap: Failed to copy k_q1 to user space\n");
        return -EFAULT;
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3326_sqrt_swap: Failed to copy k_q2 to user space\n");
        return -EFAULT;
    }

    return 0; // Success
}

#endif
