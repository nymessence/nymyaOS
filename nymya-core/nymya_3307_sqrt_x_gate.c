// src/nymya_3307_sqrt_x_gate.c

#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
#endif

#ifndef __KERNEL__

/*
 * User-space √X gate: multiplies qubit amplitude by (1/√2)(1 + i).
 */
int nymya_3307_sqrt_x_gate(nymya_qubit* q) {
    if (!q) return -1;

    _Complex double phase = (1.0 / sqrt(2.0)) * (1.0 + I);
    q->amplitude *= phase;

    log_symbolic_event("SQRT_X", q->id, q->tag, "Applied √X gate (liminal rotation)");
    return 0;
}

#else

/*
 * Kernel syscall: √X gate using fixed-point arithmetic.
 */
SYSCALL_DEFINE1(nymya_3307_sqrt_x_gate, struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;
    int64_t re, im;
    // fixed-point approximation of 1/sqrt(2)
    const int64_t c_re = (int64_t)(0.70710678 * FIXED_POINT_SCALE);
    const int64_t c_im = (int64_t)(0.70710678 * FIXED_POINT_SCALE);

    if (!user_q)
        return -EINVAL;
    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    re = kq.amplitude.re;
    im = kq.amplitude.im;

    // Fixed-point complex multiplication:
    // new_re = re*c_re - im*c_im
    // new_im = re*c_im + im*c_re
    __int128 temp_re = (__int128)re * c_re - (__int128)im * c_im;
    __int128 temp_im = (__int128)re * c_im + (__int128)im * c_re;

    kq.amplitude.re = (int64_t)(temp_re >> 32);
    kq.amplitude.im = (int64_t)(temp_im >> 32);

    log_symbolic_event("SQRT_X", kq.id, kq.tag, "Applied √X gate (liminal rotation)");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif

