// src/nymya_3338_givens.c

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

int nymya_3338_givens(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    if (!q1 || !q2) return -1;

    _Complex double a = q1->amplitude, b = q2->amplitude;
    q1->amplitude = a * cos(theta) - b * sin(theta);
    q2->amplitude = a * sin(theta) + b * cos(theta);

    log_symbolic_event("GIVENS", q1->id, q1->tag, "Givens rotation applied");
    return 0;
}

#else

SYSCALL_DEFINE3(nymya_3338_givens,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    double, theta) {

    struct nymya_qubit k_q1, k_q2;

    if (!user_q1 || !user_q2)
        return -EINVAL;

    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;

    int ret = nymya_3338_givens(&k_q1, &k_q2, theta);

    if (ret == 0)
        log_symbolic_event("GIVENS", k_q1.id, k_q1.tag, "Givens rotation applied");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return ret;
}

#endif
