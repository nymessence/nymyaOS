// src/nymya_3315_phase_shift.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>
#define __NR_nymya_3315_phase_shift NYMYA_PHASE_SHIFT_CODE

    #include <stdlib.h>
    #include <math.h>
    #include <complex.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/module.h> // Required for EXPORT_SYMBOL_GPL
#endif

/**
 * nymya_3315_phase_shift - Applies a variable phase shift to a qubit's amplitude.
 * @q: Pointer to the target qubit.
 * @theta: Phase shift angle in radians.
 *
 * In user-space, multiplies the qubit amplitude by e^(i * theta),
 * using native complex arithmetic.
 *
 * Returns:
 *   0 on success,
 *  -1 if q is NULL.
 */
#ifndef __KERNEL__
int nymya_3315_phase_shift(nymya_qubit* q, double theta)
{
    if (!q)
        return -1;

    q->amplitude *= cexp(I * theta);
    log_symbolic_event("PHASE_SHIFT", q->id, q->tag, "Applied variable phase shift");
    return 0;
}

#else

/**
 * nymya_3315_phase_shift - Applies a variable phase shift to a kernel-space qubit's amplitude.
 * @kq: Pointer to the kernel-space target nymya_qubit struct.
 * @theta_fixed: Phase shift angle in Q32.32 fixed-point format (int64_t).
 *
 * This function encapsulates the core logic for the phase shift operation
 * within the kernel. It computes the fixed-point cosine and sine of the
 * given angle, constructs a fixed-point complex phase factor, multiplies
 * the qubit's amplitude by this factor, and logs the operation.
 *
 * Returns:
 *   0 on success.
 */
int nymya_3315_phase_shift(struct nymya_qubit *kq, int64_t theta_fixed)
{
    complex_double phase;

    /* Build the fixed-point phase factor using Q32.32 fixed-point trig */
    phase.re = fixed_cos(theta_fixed);
    phase.im = fixed_sin(theta_fixed);

    /* Multiply amplitude by phase factor in fixed-point */
    kq->amplitude = complex_mul(kq->amplitude, phase);

    /* Log the event */
    log_symbolic_event("PHASE_SHIFT", kq->id, kq->tag, "Applied variable phase shift");

    return 0; // Currently, this function does not have an explicit failure mode.
}
EXPORT_SYMBOL_GPL(nymya_3315_phase_shift);

/**
 * SYSCALL_DEFINE2(nymya_3315_phase_shift) - Kernel syscall to apply a phase shift.
 * @user_q: User-space pointer to the nymya_qubit struct.
 * @theta_fixed: Phase shift angle in Q32.32 fixed-point format (int64_t).
 *
 * Copies the qubit from user space, computes the fixed‑point
 * cosine and sine via fixed_cos()/fixed_sin(), builds a
 * fixed‑point complex multiplier, multiplies the amplitude,
 * logs the event, and writes back.
 *
 * Returns:
 *   0 on success,
 *  -EINVAL if user_q is NULL,
 *  -EFAULT if copy_from_user or copy_to_user fails.
 */
SYSCALL_DEFINE2(nymya_3315_phase_shift,
               struct nymya_qubit __user *, user_q,
               int64_t, theta_fixed)
{
    struct nymya_qubit k_q;
    int ret;

    /* Validate user pointer */
    if (!user_q)
        return -EINVAL;

    /* Copy in the qubit struct from user space */
    if (copy_from_user(&k_q, user_q, sizeof(k_q)))
        return -EFAULT;

    /* Call the newly extracted core logic function with kernel-space variables */
    ret = nymya_3315_phase_shift(&k_q, theta_fixed);
    if (ret) {
        /* Propagate error from the core function if it ever fails */
        return ret;
    }

    /* Copy back the modified qubit struct to user space */
    if (copy_to_user(user_q, &k_q, sizeof(k_q)))
        return -EFAULT;

    return 0;
}

#endif
