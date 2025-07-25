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
    complex_double phase;

    /* Validate user pointer */
    if (!user_q)
        return -EINVAL;

    /* Copy in the qubit struct */
    if (copy_from_user(&k_q, user_q, sizeof(k_q)))
        return -EFAULT;

    /* Build the fixed-point phase factor using Q32.32 fixed-point trig */
    phase.re = fixed_cos(theta_fixed);
    phase.im = fixed_sin(theta_fixed);

    /* Multiply amplitude by phase factor in fixed-point */
    k_q.amplitude = complex_mul(k_q.amplitude, phase);

    /* Log the event */
    log_symbolic_event("PHASE_SHIFT", k_q.id, k_q.tag, "Applied variable phase shift");

    /* Copy back to user space */
    if (copy_to_user(user_q, &k_q, sizeof(k_q)))
        return -EFAULT;

    return 0;
}

#endif

