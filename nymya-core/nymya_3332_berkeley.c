// src/nymya_3332_berkeley.c

#include <stdio.h>
#include <stdlib.h>

#include "nymya.h"

#ifndef __KERNEL__
    // Userland implementation
int nymya_3332_berkeley(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    if (!q1 || !q2) return -1;

    nymya_3309_controlled_not(q1, q2);
    nymya_3316_phase_gate(q2, theta);
    nymya_3309_controlled_not(q1, q2);

    log_symbolic_event("BERKELEY", q1->id, q1->tag, "Berkeley entangler applied");
    return 0;
}

#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>

SYSCALL_DEFINE3(nymya_3332_berkeley,
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

    int ret = nymya_3332_berkeley(&k_q1, &k_q2, theta);

    if (ret == 0)
        log_symbolic_event("BERKELEY", k_q1.id, k_q1.tag, "Berkeley entangler applied");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return ret;
}
#endif
