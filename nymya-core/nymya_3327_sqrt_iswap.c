// src/nymya_3327_sqrt_iswap.c

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

int nymya_3327_sqrt_iswap(nymya_qubit* q1, nymya_qubit* q2) {
    if (!q1 || !q2) return -1;

    _Complex double a = q1->amplitude;
    _Complex double b = q2->amplitude;

    q1->amplitude = (a + I * b) / sqrt(2.0);
    q2->amplitude = (b + I * a) / sqrt(2.0);

    log_symbolic_event("√iSWAP", q2->id, q2->tag, "√iSWAP applied");
    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3327_sqrt_iswap,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2;

    if (!user_q1 || !user_q2)
        return -EINVAL;

    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;

    _Complex double a = k_q1.amplitude;
    _Complex double b = k_q2.amplitude;

    k_q1.amplitude = (a + I * b) / sqrt(2.0);
    k_q2.amplitude = (b + I * a) / sqrt(2.0);

    log_symbolic_event("√iSWAP", k_q2.id, k_q2.tag, "√iSWAP applied");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return 0;
}

#endif
