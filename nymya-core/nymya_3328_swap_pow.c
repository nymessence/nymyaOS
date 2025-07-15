// src/nymya_3328_swap_pow.c

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

int nymya_3328_swap_pow(nymya_qubit* q1, nymya_qubit* q2, double alpha) {
    if (!q1 || !q2) return -1;

    _Complex double a = q1->amplitude;
    _Complex double b = q2->amplitude;

    double c = cos(alpha * M_PI / 2.0);
    double s = sin(alpha * M_PI / 2.0);

    q1->amplitude = c * a + s * b;
    q2->amplitude = c * b + s * a;

    log_symbolic_event("SWAP^α", q1->id, q1->tag, "Interpolated SWAP applied");
    return 0;
}

#else

SYSCALL_DEFINE3(nymya_3328_swap_pow,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    double, alpha) {

    struct nymya_qubit k_q1, k_q2;

    if (!user_q1 || !user_q2)
        return -EINVAL;

    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;

    double c = cos(alpha * M_PI / 2.0);
    double s = sin(alpha * M_PI / 2.0);

    _Complex double a = k_q1.amplitude;
    _Complex double b = k_q2.amplitude;

    k_q1.amplitude = c * a + s * b;
    k_q2.amplitude = c * b + s * a;

    log_symbolic_event("SWAP^α", k_q1.id, k_q1.tag, "Interpolated SWAP applied");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return 0;
}

#endif
