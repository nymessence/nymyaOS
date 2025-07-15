// src/nymya_3322_xx_interaction.c

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

int nymya_3322_xx_interaction(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    if (!q1 || !q2) return -1;

    _Complex double phase = cos(theta) + I * sin(theta);
    q1->amplitude *= phase;
    q2->amplitude *= phase;

    log_symbolic_event("XX", q1->id, q1->tag, "Applied XX interaction with partner");
    return 0;
}

#else

SYSCALL_DEFINE3(nymya_3322_xx_interaction,
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

    double cos_val = cos(theta);
    double sin_val = sin(theta);
    _Complex double phase = cos_val + sin_val * I;

    k_q1.amplitude *= phase;
    k_q2.amplitude *= phase;

    log_symbolic_event("XX", k_q1.id, k_q1.tag, "Applied XX interaction with partner");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return 0;
}

#endif
