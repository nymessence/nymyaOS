// src/nymya_3318_controlled_phase_s.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>
#define __NR_nymya_3318_controlled_phase_s NYMYA_CPHASE_S_CODE

    #include <stdlib.h>
    #include <math.h>
    #include <complex.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/module.h> // Added for EXPORT_SYMBOL_GPL
#endif

/**
 * nymya_3318_controlled_phase_s - Applies a controlled S phase (π/2) gate on target qubit.
 *
 * This function applies a conditional phase shift of π/2 radians (the S gate) to the
 * target qubit amplitude only if the control qubit amplitude magnitude exceeds 0.5.
 *
 * In user-space, it uses native complex arithmetic with double precision floating-point.
 *
 * In kernel-space, it uses fixed-point arithmetic (Q32.32) with no floating-point or
 * complex types, to abide by kernel restrictions.
 *
 * Both implementations log symbolic events for traceability and debugging.
 *
 * @qc: Pointer to the control qubit (nymya_qubit struct).
 * @qt: Pointer to the target qubit (nymya_qubit struct).
 *
 * Returns:
 * - In user-space:
 *   - 0 on success.
 *   - -1 if either qubit pointer is NULL.
 * - In kernel-space:
 *   - 0 on success.
 *   - -EINVAL if either user pointer is NULL.
 *   - -EFAULT if copy_from_user or copy_to_user fails.
 */

#ifndef __KERNEL__

int nymya_3318_controlled_phase_s(nymya_qubit* qc, nymya_qubit* qt) {
    // Check for null pointers
    if (!qc || !qt)
        return -1;

    // If control qubit amplitude magnitude > 0.5, apply S phase shift to target
    if (cabs(qc->amplitude) > 0.5) {
        // Multiply target amplitude by e^(i * π/2) = i (S gate)
        qt->amplitude *= cexp(I * (M_PI / 2.0));
        log_symbolic_event("C-PHASE-S", qt->id, qt->tag, "Conditional S phase applied");
    } else {
        // No action; control qubit amplitude too small
        log_symbolic_event("C-PHASE-S", qt->id, qt->tag, "No action (control = 0)");
    }

    return 0;
}

#else

/**
 * @brief Kernel-side core logic for nymya_3318_controlled_phase_s.
 *
 * This function applies a conditional S phase (pi/2) gate to the target qubit
 * based on the control qubit's amplitude magnitude. It operates on kernel-space
 * nymya_qubit structures using fixed-point arithmetic.
 *
 * @param k_qc Pointer to the kernel-space control qubit data.
 * @param k_qt Pointer to the kernel-space target qubit data. The amplitude of this
 *             qubit may be modified if the condition is met.
 * @return 0 on success. Currently, no specific error conditions are handled within
 *         this core logic, assuming valid kernel-space pointers are provided.
 */
int nymya_3318_controlled_phase_s_core(struct nymya_qubit *k_qc, struct nymya_qubit *k_qt) {
    complex_double phase;
    __int128 re_sq, im_sq, mag_sq;
    __int128 threshold;

    /*
     * Calculate squared magnitude of control qubit amplitude in fixed-point Q32.32:
     * magnitude^2 = re^2 + im^2
     * Using 128-bit intermediate to avoid overflow.
     */
    re_sq = (__int128)k_qc->amplitude.re * k_qc->amplitude.re;
    im_sq = (__int128)k_qc->amplitude.im * k_qc->amplitude.im;
    mag_sq = re_sq + im_sq;

    /*
     * Threshold for magnitude squared corresponds to (0.5)^2 = 0.25 in fixed-point:
     * 0.5 in Q32.32 = FIXED_POINT_SCALE / 2
     * So threshold = (FIXED_POINT_SCALE / 2)^2
     */
    threshold = (__int128)(FIXED_POINT_SCALE / 2) * (FIXED_POINT_SCALE / 2);

    if (mag_sq > threshold) {
        /*
         * Apply S phase shift (π/2):
         * cos(π/2) = 0
         * sin(π/2) = 1
         * So phase factor = 0 + i*1 in fixed-point.
         */
        phase.re = 0;
        phase.im = FIXED_POINT_SCALE;

        // Multiply target amplitude by phase factor (fixed-point complex multiplication)
        k_qt->amplitude = complex_mul(k_qt->amplitude, phase);

        log_symbolic_event("C-PHASE-S", k_qt->id, k_qt->tag, "Conditional S phase applied");
    } else {
        // Control amplitude too low; no action
        log_symbolic_event("C-PHASE-S", k_qt->id, k_qt->tag, "No action (control = 0)");
    }

    return 0; // Success
}
EXPORT_SYMBOL_GPL(nymya_3318_controlled_phase_s_core);

SYSCALL_DEFINE2(nymya_3318_controlled_phase_s,
    struct nymya_qubit __user *, user_qc,
    struct nymya_qubit __user *, user_qt)
{
    struct nymya_qubit k_qc, k_qt;
    int ret;

    // Validate user pointers
    if (!user_qc || !user_qt)
        return -EINVAL;

    // Copy control qubit data from user space to kernel space
    if (copy_from_user(&k_qc, user_qc, sizeof(k_qc)))
        return -EFAULT;

    // Copy target qubit data from user space to kernel space
    if (copy_from_user(&k_qt, user_qt, sizeof(k_qt)))
        return -EFAULT;

    // Call the new core logic function
    ret = nymya_3318_controlled_phase_s_core(&k_qc, &k_qt);
    if (ret) // Propagate error from core if it ever returns one
        return ret;

    // Copy modified target qubit data back to user space
    if (copy_to_user(user_qt, &k_qt, sizeof(k_qt)))
        return -EFAULT;

    return 0;
}

#endif
