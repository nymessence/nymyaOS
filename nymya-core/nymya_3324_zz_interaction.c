// src/nymya_3324_zz_interaction.c

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

int nymya_3324_zz_interaction(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    if (!q1 || !q2) return -1;

    _Complex double zphase = cexp(I * theta);
    q1->amplitude *= zphase;
    q2->amplitude *= zphase;

    log_symbolic_event("ZZ", q2->id, q2->tag, "Applied ZZ phase coupling");
    return 0;
}

#else

SYSCALL_DEFINE3(nymya_3324_zz_interaction,
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

    _Complex double zphase = cos(theta) + I * sin(theta);
    k_q1.amplitude *= zphase;
    k_q2.amplitude *= zphase;

    log_symbolic_event("ZZ", k_q2.id, k_q2.tag, "Applied ZZ phase coupling");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return 0;
}

#endif
