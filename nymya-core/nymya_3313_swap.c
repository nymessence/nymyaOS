// src/nymya_3313_swap.c

#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
#endif

#ifndef __KERNEL__

int nymya_3313_swap(nymya_qubit* q1, nymya_qubit* q2) {
    if (!q1 || !q2) return -1;

    _Complex double tmp = q1->amplitude;
    q1->amplitude = q2->amplitude;
    q2->amplitude = tmp;

    log_symbolic_event("SWAP", q1->id, q1->tag, "Swapped with pair");
    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3313_swap,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2;

    if (!user_q1 || !user_q2)
        return -EINVAL;

    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;

    _Complex double tmp = k_q1.amplitude;
    k_q1.amplitude = k_q2.amplitude;
    k_q2.amplitude = tmp;

    log_symbolic_event("SWAP", k_q1.id, k_q1.tag, "Swapped with pair");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return 0;
}

#endif
