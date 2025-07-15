// src/nymya_3319_rotate_x.c

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

int nymya_3319_rotate_x(nymya_qubit* q, double theta) {
    if (!q) return -1;

    q->amplitude *= cos(theta / 2.0) + I * sin(theta / 2.0);

    log_symbolic_event("ROT_X", q->id, q->tag, "Applied X-axis rotation");
    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3319_rotate_x,
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

    log_symbolic_event("ROT_X", k_q.id, k_q.tag, "Applied X-axis rotation");

    if (copy_to_user(user_q, &k_q, sizeof(k_q)))
        return -EFAULT;

    return 0;
}

#endif
