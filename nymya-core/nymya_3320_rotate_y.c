// src/nymya_3320_rotate_y.c

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

int nymya_3320_rotate_y(nymya_qubit* q, double theta) {
    if (!q) return -1;

    q->amplitude *= cos(theta / 2.0) + I * sin(theta / 2.0);

    log_symbolic_event("ROT_Y", q->id, q->tag, "Applied Y-axis rotation");
    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3320_rotate_y,
    struct nymya_qubit __user *, user_q,
    double, theta) {

    struct nymya_qubit k_q;

    if (!user_q)
        return -EINVAL;

    if (copy_from_user(&k_q, user_q, sizeof(k_q)))
        return -EFAULT;

    double cos_val = cos(theta / 2.0);
    double sin_val = sin(theta / 2.0);
    _Complex double phase = cos_val + sin_val * I;

    k_q.amplitude *= phase;

    log_symbolic_event("ROT_Y", k_q.id, k_q.tag, "Applied Y-axis rotation");

    if (copy_to_user(user_q, &k_q, sizeof(k_q)))
        return -EFAULT;

    return 0;
}

#endif
