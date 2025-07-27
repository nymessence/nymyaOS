// src/nymya_3312_double_controlled_not.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_nymya_3312_double_controlled_not NYMYA_DCNOT_CODE

    #include <stdio.h>
    #include <stdlib.h>
    #include <complex.h>
#else
    int nymya_3312_double_controlled_not_core(struct nymya_qubit *qc1, struct nymya_qubit *qc2, struct nymya_qubit *qt);


    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/module.h> // Required for EXPORT_SYMBOL_GPL
#endif

/**
 * nymya_3312_double_controlled_not - Double-controlled NOT (Toffoli-like) gate.
 * @qc1: Pointer to first control qubit.
 * @qc2: Pointer to second control qubit.
 * @qt: Pointer to target qubit.
 *
 * Applies a NOT operation (phase flip) on the target qubit if
 * both control qubits have amplitude magnitude > 0.5.
 *
 * Returns 0 on success, negative error code on failure.
 */
#ifndef __KERNEL__

int nymya_3312_double_controlled_not(nymya_qubit* qc1, nymya_qubit* qc2, nymya_qubit* qt) {
    if (!qc1 || !qc2 || !qt) return -1;

    if (cabs(qc1->amplitude) > 0.5 && cabs(qc2->amplitude) > 0.5) {
        qt->amplitude *= -1;
        log_symbolic_event("DCNOT", qt->id, qt->tag, "Double control triggered NOT");
    } else {
        log_symbolic_event("DCNOT", qt->id, qt->tag, "Conditions not met");
    }

    return 0;
}

#else

/**
 * @brief Kernel-side core logic for the nymya_3312_double_controlled_not gate.
 *
 * This function applies a NOT operation (phase flip) on the target qubit if
 * both control qubits' amplitude magnitudes exceed a predefined threshold.
 * It operates on kernel-space qubit structures, modifying the target qubit
 * in place if conditions are met.
 *
 * @param qc1 Pointer to the first control qubit (kernel-space).
 * @param qc2 Pointer to the second control qubit (kernel-space).
 * @param qt Pointer to the target qubit (kernel-space). The amplitude of
 *           this qubit might be modified.
 * @return 0 on success, negative error code on failure.
 */
int nymya_3312_double_controlled_not_core(struct nymya_qubit *qc1,
                                          struct nymya_qubit *qc2,
                                          struct nymya_qubit *qt) {
    int64_t re1, im1, re2, im2;
    uint64_t re1_abs, im1_abs, re2_abs, im2_abs;
    uint64_t mag1_sq, mag2_sq;
    // FIXED_POINT_SCALE is assumed to be defined globally or in nymya.h
    const uint64_t threshold_sq = (FIXED_POINT_SCALE >> 1) * (FIXED_POINT_SCALE >> 1) / FIXED_POINT_SCALE;

    // Compute magnitude squared for first control qubit
    re1 = qc1->amplitude.re;
    im1 = qc1->amplitude.im;
    re1_abs = (re1 < 0) ? -re1 : re1;
    im1_abs = (im1 < 0) ? -im1 : im1;
    mag1_sq = ((uint64_t)re1_abs * (uint64_t)re1_abs >> 32) + ((uint64_t)im1_abs * (uint64_t)im1_abs >> 32);

    // Compute magnitude squared for second control qubit
    re2 = qc2->amplitude.re;
    im2 = qc2->amplitude.im;
    re2_abs = (re2 < 0) ? -re2 : re2;
    im2_abs = (im2 < 0) ? -im2 : im2;
    mag2_sq = ((uint64_t)re2_abs * (uint64_t)re2_abs >> 32) + ((uint64_t)im2_abs * (uint64_t)im2_abs >> 32);

    if (mag1_sq > threshold_sq && mag2_sq > threshold_sq) {
        qt->amplitude.re = -qt->amplitude.re;
        qt->amplitude.im = -qt->amplitude.im;
        log_symbolic_event("DCNOT", qt->id, qt->tag, "Double control triggered NOT");
    } else {
        log_symbolic_event("DCNOT", qt->id, qt->tag, "Conditions not met");
    }

    return 0; // Current logic always succeeds.
}
EXPORT_SYMBOL_GPL(nymya_3312_double_controlled_not_core);


SYSCALL_DEFINE3(nymya_3312_double_controlled_not,
    struct nymya_qubit __user *, user_qc1,
    struct nymya_qubit __user *, user_qc2,
    struct nymya_qubit __user *, user_qt) {

    struct nymya_qubit k_qc1, k_qc2, k_qt; // Kernel-space copies
    int ret;

    if (!user_qc1 || !user_qc2 || !user_qt)
        return -EINVAL;

    if (copy_from_user(&k_qc1, user_qc1, sizeof(k_qc1)))
        return -EFAULT;
    if (copy_from_user(&k_qc2, user_qc2, sizeof(k_qc2)))
        return -EFAULT;
    if (copy_from_user(&k_qt, user_qt, sizeof(k_qt)))
        return -EFAULT;

    // Call the newly created core function
    ret = nymya_3312_double_controlled_not_core(&k_qc1, &k_qc2, &k_qt);
    if (ret) {
        // Propagate error from core function if any
        return ret;
    }

    // Copy back the potentially modified target qubit to user space
    if (copy_to_user(user_qt, &k_qt, sizeof(k_qt)))
        return -EFAULT;

    return 0;
}

#endif