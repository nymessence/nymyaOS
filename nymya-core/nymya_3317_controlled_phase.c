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

#else

SYSCALL_DEFINE3(nymya_3317_controlled_phase,
    struct nymya_qubit __user *, user_qc,
    struct nymya_qubit __user *, user_qt,
    int64_t, theta_fixed) // theta_fixed in Q32.32 fixed-point
{
    struct nymya_qubit k_qc, k_qt;
    complex_double phase;

    if (!user_qc || !user_qt)
        return -EINVAL;

    if (copy_from_user(&k_qc, user_qc, sizeof(k_qc)))
        return -EFAULT;
    if (copy_from_user(&k_qt, user_qt, sizeof(k_qt)))
        return -EFAULT;

    // Calculate magnitude of control amplitude squared (re^2 + im^2)
    // Use 128-bit math for safety (fixed-point Q32.32)
    __int128 re_sq = (__int128)k_qc.amplitude.re * k_qc.amplitude.re;
    __int128 im_sq = (__int128)k_qc.amplitude.im * k_qc.amplitude.im;
    __int128 mag_sq = re_sq + im_sq;

    // Threshold: 0.5 magnitude means mag_sq > (0.5 * FIXED_POINT_SCALE)^2
    // 0.5 in fixed-point = FIXED_POINT_SCALE / 2
    __int128 threshold = (__int128)(FIXED_POINT_SCALE / 2) * (FIXED_POINT_SCALE / 2);

    if (mag_sq > threshold) {
        // Build phase multiplier
        phase.re = fixed_cos(theta_fixed);
        phase.im = fixed_sin(theta_fixed);

        k_qt.amplitude = complex_mul(k_qt.amplitude, phase);
        log_symbolic_event("C-PHASE", k_qt.id, k_qt.tag, "Controlled phase applied");
    } else {
        log_symbolic_event("C-PHASE", k_qt.id, k_qt.tag, "No action (control = 0)");
    }

    if (copy_to_user(user_qt, &k_qt, sizeof(k_qt)))
        return -EFAULT;

    return 0;
}

#endif

