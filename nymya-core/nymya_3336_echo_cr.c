// src/nymya_3336_echo_cr.c

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

int nymya_3336_echo_cr(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    if (!q1 || !q2) return -1;

    _Complex double p = cexp(I * theta);
    q1->amplitude *= p;
    q2->amplitude *= conj(p);
    q1->amplitude *= conj(p);
    q2->amplitude *= p;

    log_symbolic_event("ECHO_CR", q1->id, q1->tag, "ECR interaction applied");
    return 0;
}

#else

SYSCALL_DEFINE3(nymya_3336_echo_cr,
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

    int ret = nymya_3336_echo_cr(&k_q1, &k_q2, theta);

    if (ret == 0)
        log_symbolic_event("ECHO_CR", k_q1.id, k_q1.tag, "ECR interaction applied");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return ret;
}

#endif
