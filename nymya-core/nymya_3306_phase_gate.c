// src/nymya_3306_phase_gate.c

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
 * User-space implementation of Phase (S) gate.
 * Multiplies amplitude by exp(i * π/2) == i.
 */
int nymya_3306_phase_gate(nymya_qubit* q) {
    if (!q) return -1;

    q->amplitude *= cexp(I * M_PI_2);

    log_symbolic_event("PHASE_S", q->id, q->tag, "Applied S gate (π/2 phase)");
    return 0;
}

#else

/*
 * Kernel syscall: nymya_3306_phase_gate
 * Multiplies qubit amplitude by fixed-point i (equivalent to phase shift π/2).
 */
SYSCALL_DEFINE1(nymya_3306_phase_gate, struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;
    int64_t re, im;

    if (!user_q)
        return -EINVAL;

    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    re = kq.amplitude.re;
    im = kq.amplitude.im;

    // (a + bi) * i = -b + ai
    kq.amplitude.re = -im;
    kq.amplitude.im = re;

    log_symbolic_event("PHASE_S", kq.id, kq.tag, "Applied S gate (π/2 phase)");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif

