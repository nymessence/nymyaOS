// src/nymya_3316_phase_gate.c

#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
#endif

#ifndef __KERNEL__

int nymya_3316_phase_gate(nymya_qubit* q, double phi) {
    if (!q) return -1;

    q->amplitude *= cexp(I * phi);

    log_symbolic_event("PHASE_GATE", q->id, q->tag, "Applied symbolic phase gate");
    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3316_phase_gate,
    struct nymya_qubit __user *, user_q,
    double, phi) {

    struct nymya_qubit k_q;

    if (!user_q)
        return -EINVAL;

    if (copy_from_user(&k_q, user_q, sizeof(k_q)))
        return -EFAULT;

    double cos_phi = cos(phi);
    double sin_phi = sin(phi);

    _Complex double phase = cos_phi + sin_phi * I;

    k_q.amplitude *= phase;

    log_symbolic_event("PHASE_GATE", k_q.id, k_q.tag, "Applied symbolic phase gate");

    if (copy_to_user(user_q, &k_q, sizeof(k_q)))
        return -EFAULT;

    return 0;
}

#endif
