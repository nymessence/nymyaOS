// src/nymya_3321_rotate_z.c

#include <stdint.h>
#include <complex.h>
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

int nymya_3321_rotate_z(nymya_qubit* q, double theta) {
    if (!q) return -1;

    q->amplitude *= cexp(I * theta);

    log_symbolic_event("ROT_Z", q->id, q->tag, "Applied Z-axis rotation");
    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3321_rotate_z,
    struct nymya_qubit __user *, user_q,
    double, theta) {

    struct nymya_qubit k_q;

    if (!user_q)
        return -EINVAL;

    if (copy_from_user(&k_q, user_q, sizeof(k_q)))
        return -EFAULT;

    double cos_val = cos(theta);
    double sin_val = sin(theta);
    _Complex double phase = cos_val + sin_val * I;

    k_q.amplitude *= phase;

    log_symbolic_event("ROT_Z", k_q.id, k_q.tag, "Applied Z-axis rotation");

    if (copy_to_user(user_q, &k_q, sizeof(k_q)))
        return -EFAULT;

    return 0;
}

#endif
