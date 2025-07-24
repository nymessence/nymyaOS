// src/nymya_3316_phase_gate.c

#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <complex.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/types.h> // For __int128 and other kernel types
    // No math.h or complex.h in kernel
#endif

// The fixed-point constants and helper functions (fixed_point_mul, fixed_point_cos,
// fixed_point_sin, make_complex, complex_mul, complex_exp_i, complex_re,
// complex_im, complex_conj) are now defined as static inline functions
// within the #ifdef __KERNEL__ block in nymya.h.
// Therefore, they should NOT be redefined here.

/**
 * nymya_3316_phase_gate - Applies a symbolic phase gate to a qubit's amplitude.
 * @q: Pointer to the target qubit.
 * @phi: Phase angle in radians.
 *
 * User-space multiplies the qubit amplitude by e^(i * phi) using native complex math.
 * Kernel-space multiplies by fixed-point complex multiplier using fixed_point_cos and fixed_point_sin.
 *
 * Returns:
 * 0 on success,
 * -1 if the qubit pointer is NULL (user-space),
 * -EINVAL if user pointer is NULL (kernel-space),
 * -EFAULT on copy failures (kernel-space).
 */
#ifndef __KERNEL__

int nymya_3316_phase_gate(nymya_qubit* q, double phi) {
    if (!q)
        return -1;

    // User-space uses standard complex math library
    q->amplitude *= cexp(I * phi);

    log_symbolic_event("PHASE_GATE", q->id, q->tag, "Applied symbolic phase gate");
    return 0;
}

#else // __KERNEL__

SYSCALL_DEFINE2(nymya_3316_phase_gate,
    struct nymya_qubit __user *, user_q,
    int64_t, phi_fixed)  // phi_fixed is Q32.32 fixed-point angle
{
    struct nymya_qubit k_q;
    complex_double phase_factor;

    if (!user_q)
        return -EINVAL;

    if (copy_from_user(&k_q, user_q, sizeof(k_q)))
        return -EFAULT;

    // Build fixed-point complex phase factor: cos(phi) + i * sin(phi)
    // These functions (fixed_point_cos, fixed_point_sin) are provided by nymya.h
    phase_factor.re = fixed_point_cos(phi_fixed);
    phase_factor.im = fixed_point_sin(phi_fixed);

    // Multiply amplitude by phase factor using fixed-point complex multiplication
    // complex_mul is provided by nymya.h
    k_q.amplitude = complex_mul(k_q.amplitude, phase_factor);

    log_symbolic_event("PHASE_GATE", k_q.id, k_q.tag, "Applied symbolic phase gate");

    if (copy_to_user(user_q, &k_q, sizeof(k_q)))
        return -EFAULT;

    return 0;
}

#endif

