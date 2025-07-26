// src/nymya_3317_controlled_phase.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>
#define __NR_nymya_3317_controlled_phase NYMYA_CPHASE_CODE

    #include <stdlib.h>
    #include <math.h>
    #include <complex.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/module.h> // ADDED: Required for EXPORT_SYMBOL_GPL
#endif

/**
 * nymya_3317_controlled_phase - Applies a controlled phase rotation on target qubit.
 * @qc: Pointer to the control qubit.
 * @qt: Pointer to the target qubit.
 * @theta: Phase angle in radians.
 *
 * User-space: multiplies target amplitude by e^(i*theta) if control magnitude > 0.5.
 * Kernel-space: uses fixed-point trig & multiplication; theta is fixed-point Q32.32.
 *
 * Returns:
 * 0 on success,
 * -1 if qubit pointers NULL (user-space),
 * -EINVAL if user pointers NULL (kernel-space),
 * -EFAULT on copy failures (kernel-space).
 */
#ifndef __KERNEL__

int nymya_3317_controlled_phase(nymya_qubit* qc, nymya_qubit* qt, double theta) {
    if (!qc || !qt)
        return -1;

    if (cabs(qc->amplitude) > 0.5) {
        qt->amplitude *= cexp(I * theta);
        log_symbolic_event("C-PHASE", qt->id, qt->tag, "Controlled phase applied");
    } else {
        log_symbolic_event("C-PHASE", qt->id, qt->tag, "No action (control = 0)");
    }

    return 0;
}

#else // __KERNEL__

/**
 * nymya_3317_controlled_phase - Core kernel function for Controlled-Phase gate.
 * @k_qc: Pointer to the kernel-space control qubit.
 * @k_qt: Pointer to the kernel-space target qubit.
 * @theta_fixed: Phase angle in fixed-point (int64_t) format.
 *
 * This function applies a controlled phase rotation to the target qubit.
 * If the magnitude of the control qubit's amplitude exceeds 0.5 (in fixed-point),
 * it applies a phase rotation to the target qubit's amplitude.
 * This function is designed to be called directly by other kernel code.
 *
 * Returns 0 on success, -EINVAL on null input.
 */
int nymya_3317_controlled_phase(struct nymya_qubit *k_qc, struct nymya_qubit *k_qt, int64_t theta_fixed) {
    complex_double phase;
    int64_t re, im;
    uint64_t re64, im64;
    uint64_t re_sq, im_sq;
    uint64_t mag_sq;

    if (!k_qc || !k_qt) {
        pr_err("NYMYA: nymya_3317_controlled_phase received NULL kernel qubit pointer(s)\n");
        return -EINVAL;
    }

    // Extract real and imaginary parts of control qubit
    re = k_qc->amplitude.re;
    im = k_qc->amplitude.im;

    re64 = (uint64_t)(re < 0 ? -re : re);
    im64 = (uint64_t)(im < 0 ? -im : im);

    // Compute magnitude^2 using fixed-point math
    re_sq = ((__uint128_t)re64 * re64) >> 32;
    im_sq = ((__uint128_t)im64 * im64) >> 32;
    mag_sq = re_sq + im_sq;

    // Fixed-point threshold for 0.5^2 (0.25 * FIXED_POINT_SCALE)
    const uint64_t threshold_sq = (FIXED_POINT_SCALE / 4);

    if (mag_sq > threshold_sq) {
        // Build phase multiplier
        phase.re = fixed_cos(theta_fixed);
        phase.im = fixed_sin(theta_fixed);

        k_qt->amplitude = complex_mul(k_qt->amplitude, phase);
        log_symbolic_event("C-PHASE", k_qt->id, k_qt->tag, "Controlled phase applied");
    } else {
        log_symbolic_event("C-PHASE", k_qt->id, k_qt->tag, "No action (control = 0)");
    }

    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3317_controlled_phase);



// Export the symbol for this function so other kernel modules/code can call it directly.


/**
 * SYSCALL_DEFINE3(nymya_3317_controlled_phase) - Kernel implementation of Controlled-Z gate.
 * @user_qc: User pointer to the control qubit structure.
 * @user_qt: User pointer to the target qubit structure.
 * @theta_fixed: Phase angle in fixed-point (int64_t) format.
 *
 * This system call version copies the qubit structures from user space,
 * calculates the fixed-point magnitude of the control qubit, and applies
 * a Z gate (negating the amplitude) to the target qubit if the control
 * magnitude exceeds 0.5 (in fixed-point representation).
 *
 * Returns 0 on success, -EINVAL on null input, or -EFAULT on copy errors.
 */
SYSCALL_DEFINE3(nymya_3317_controlled_phase,
    struct nymya_qubit __user *, user_qc,
    struct nymya_qubit __user *, user_qt,
    int64_t, theta_fixed) // theta_fixed in Q32.32 fixed-point
{
    struct nymya_qubit k_qc, k_qt;
    int ret;

    if (!user_qc || !user_qt)
        return -EINVAL;

    if (copy_from_user(&k_qc, user_qc, sizeof(k_qc)))
        return -EFAULT;
    if (copy_from_user(&k_qt, user_qt, sizeof(k_qt)))
        return -EFAULT;

    // Call the core logic function
    ret = nymya_3317_controlled_phase(&k_qc, &k_qt, theta_fixed);

    if (ret) // Propagate error from core function
        return ret;

    if (copy_to_user(user_qt, &k_qt, sizeof(k_qt)))
        return -EFAULT;

    return 0;
}

#endif

