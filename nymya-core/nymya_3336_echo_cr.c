// src/nymya_3336_echo_cr.c

#include "nymya.h" // Common definitions like complex_double

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>    // Userland only
#define __NR_nymya_3336_echo_cr NYMYA_ECHO_CR_CODE

    #include <stdlib.h>  // Userland only
    #include <math.h>    // For cexp, conj in userland
    #include <complex.h> // For _Complex double in userland
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h> // For -EINVAL, -EFAULT
    #include <linux/printk.h> // For pr_err
    #include <linux/module.h> // Required for EXPORT_SYMBOL_GPL
    // No math.h or complex.h in kernel; fixed-point math assumed for amplitude operations
#endif

#ifndef __KERNEL__

/**
 * nymya_3336_echo_cr - Applies an Echo Cross-Resonance (CR) gate to two qubits (userland).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @theta: The gate parameter (angle in radians).
 *
 * This function applies a sequence of complex phase rotations to the qubits' amplitudes.
 * Note: The sequence of operations (p, conj(p), conj(p), p) on q1 and q2 respectively
 * appears to result in no net change to the qubit amplitudes if 'p' is a unit complex number,
 * as p * conj(p) = |p|^2 = 1. This implementation directly mirrors the provided userland logic.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3336_echo_cr(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    if (!q1 || !q2) return -1;

    // In userland, q1->amplitude and q2->amplitude are _Complex double.
    // cexp(I * theta) computes e^(i*theta).
    _Complex double p = cexp(I * theta);

    // Apply the sequence of complex multiplications
    q1->amplitude *= p;
    q2->amplitude *= conj(p);
    q1->amplitude *= conj(p);
    q2->amplitude *= p;

    log_symbolic_event("ECHO_CR", q1->id, q1->tag, "ECR interaction applied");
    return 0;
}

#else // __KERNEL__

/**
 * @brief Applies an Echo Cross-Resonance (CR) gate to kernel-space qubits.
 * @param kq1 Pointer to the first qubit structure in kernel space.
 * @param kq2 Pointer to the second qubit structure in kernel space.
 * @param theta_fp The gate parameter (angle) in fixed-point format.
 *
 * This function implements the core logic for the Echo Cross-Resonance (CR)
 * gate. It applies a sequence of complex phase rotations to the provided
 * kernel-space qubit amplitudes using fixed-point complex arithmetic.
 *
 * This function is intended to be called by the `sys_nymya_3336_echo_cr`
 * syscall after user-space data has been copied to kernel space, or by
 * other kernel modules directly.
 *
 * @return 0 on success. Currently, no specific failure conditions are
 *         handled within this core logic that would result in a non-zero
 *         return, assuming valid input pointers.
 */
int nymya_3336_echo_cr(struct nymya_qubit *kq1, struct nymya_qubit *kq2, int64_t theta_fp) {
    // Use complex_exp_i for e^(i*theta_fp) and complex_mul for multiplications.
    complex_double p_fp = complex_exp_i(theta_fp);
    complex_double conj_p_fp = complex_conj(p_fp);

    // Apply the sequence of complex multiplications
    kq1->amplitude = complex_mul(kq1->amplitude, p_fp);
    kq2->amplitude = complex_mul(kq2->amplitude, conj_p_fp);
    kq1->amplitude = complex_mul(kq1->amplitude, conj_p_fp);
    kq2->amplitude = complex_mul(kq2->amplitude, p_fp);

    log_symbolic_event("ECHO_CR", kq1->id, kq1->tag, "ECR interaction applied");
    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3336_echo_cr);



/**
 * SYSCALL_DEFINE3(nymya_3336_echo_cr) - Kernel syscall for Echo Cross-Resonance (CR) gate.
 * @user_q1: User-space pointer to the first qubit.
 * @user_q2: User-space pointer to the second qubit.
 * @theta_fp: The gate parameter (angle) in fixed-point (int64_t) format.
 *
 * This syscall copies qubit data from user space, applies the Echo CR gate
 * logic by calling the `nymya_3336_echo_cr` core function, and then copies
 * the modified data back.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user pointer is invalid.
 * - -EFAULT if copying data to/from user space fails.
 */
SYSCALL_DEFINE3(nymya_3336_echo_cr,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    int64_t, theta_fp) { // Theta is now fixed-point

    struct nymya_qubit k_q1, k_q2; // Kernel-space copies
    int ret = 0;

    // 1. Validate user pointers
    if (!user_q1 || !user_q2) {
        pr_err("nymya_3336_echo_cr: Invalid user qubit pointers\n");
        return -EINVAL;
    }

    // 2. Copy qubit data from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3336_echo_cr: Failed to copy k_q1 from user space\n");
        return -EFAULT;
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3336_echo_cr: Failed to copy k_q2 from user space\n");
        return -EFAULT;
    }

    // 3. Call the core Echo CR logic function with kernel-space copies
    ret = nymya_3336_echo_cr(&k_q1, &k_q2, theta_fp);
    if (ret) {
        // Propagate error from core function if it ever returns one
        pr_err("nymya_3336_echo_cr: Core logic failed with error %d\n", ret);
        return ret;
    }

    // 4. Copy modified qubit data back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3336_echo_cr: Failed to copy k_q1 to user space\n");
        return -EFAULT;
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3336_echo_cr: Failed to copy k_q2 to user space\n");
        return -EFAULT;
    }

    return 0; // Return 0 for success
}

#endif
