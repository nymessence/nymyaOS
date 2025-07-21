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
#endif

#ifndef __KERNEL__

int nymya_3307_sqrt_x_gate(nymya_qubit* q) {
    if (!q) return -1;

    _Complex double phase = (1.0 / sqrt(2.0)) * (1.0 + I);
    q->amplitude *= phase;

    log_symbolic_event("SQRT_X", q->id, q->tag, "Applied √X gate (liminal rotation)");
    return 0;
}

#else

SYSCALL_DEFINE1(nymya_3307_sqrt_x_gate, struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;

    if (!user_q)
        return -EINVAL;
    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    _Complex double phase = (1.0 / sqrt(2.0)) * (1.0 + I);
    kq.amplitude *= phase;

    log_symbolic_event("SQRT_X", kq.id, kq.tag, "Applied √X gate (liminal rotation)");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif
