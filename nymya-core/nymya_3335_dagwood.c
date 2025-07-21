// src/nymya_3335_dagwood.c

#include <stdio.h>
#include <stdlib.h>

#include "nymya.h"

#ifndef __KERNEL__

int nymya_3335_dagwood(nymya_qubit* q1, nymya_qubit* q2, nymya_qubit* q3) {
    if (!q1 || !q2 || !q3) return -1;
    if (cabs(q1->amplitude) > 0.5) {
        nymya_3313_swap(q2, q3);
        log_symbolic_event("DAGWOOD", q1->id, q1->tag, "Dagwood swap applied");
    } else {
        log_symbolic_event("DAGWOOD", q1->id, q1->tag, "Control=0, no swap");
    }
    return 0;
}

#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>

SYSCALL_DEFINE3(nymya_3335_dagwood,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    struct nymya_qubit __user *, user_q3) {

    struct nymya_qubit k_q1, k_q2, k_q3;

    if (!user_q1 || !user_q2 || !user_q3)
        return -EINVAL;

    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;
    if (copy_from_user(&k_q3, user_q3, sizeof(k_q3)))
        return -EFAULT;

    int ret = nymya_3335_dagwood(&k_q1, &k_q2, &k_q3);

    if (ret == 0)
        log_symbolic_event("DAGWOOD", k_q1.id, k_q1.tag, "Dagwood swap applied");

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;
    if (copy_to_user(user_q3, &k_q3, sizeof(k_q3)))
        return -EFAULT;

    return ret;
}

#endif
