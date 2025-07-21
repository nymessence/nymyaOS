// src/nymya_3334_core_entangle.c

#include <stdio.h>
#include <stdlib.h>

#include "nymya.h"

#ifndef __KERNEL__

// Userland implementation
int nymya_3334_core_entangle(nymya_qubit* q1, nymya_qubit* q2) {
    if (!q1 || !q2) return -1;

    nymya_3308_hadamard_gate(q1);
    nymya_3309_controlled_not(q1, q2);

    log_symbolic_event("CORE_EN", q1->id, q1->tag, "Core entanglement applied");
    return 0;
}

#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>

SYSCALL_DEFINE2(nymya_3334_core_entangle,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2;

    if (!user_q1 || !user_q2)
        return -EINVAL;

    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;

    int ret = nymya_3334_core_entangle(&k_q1, &k_q2);

    if (ret == 0)
        log_symbolic_event("CORE_EN", k_q1.id, k_q1.tag, "Core entanglement applied");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return ret;
}

#endif
