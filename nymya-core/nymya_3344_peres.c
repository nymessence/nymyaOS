// src/nymya_3344_peres.c

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

int nymya_3344_peres(nymya_qubit* q1, nymya_qubit* q2, nymya_qubit* q3) {
    if (!q1 || !q2 || !q3) return -1;
    cnot(q1, q3);
    nymya_3343_margolis(q1, q2, q3);
    log_symbolic_event("PERES", q1->id, q1->tag, "Peres gate applied");
    return 0;
}

#else

SYSCALL_DEFINE3(nymya_3344_peres,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    struct nymya_qubit __user *, user_q3) {

    struct nymya_qubit k_q1, k_q2, k_q3;
    int ret;

    if (!user_q1 || !user_q2 || !user_q3)
        return -EINVAL;

    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;
    if (copy_from_user(&k_q3, user_q3, sizeof(k_q3)))
        return -EFAULT;

    ret = nymya_3309_controlled_not(&k_q1, &k_q3);  // cnot
    if (ret)
        return ret;

    ret = nymya_3343_margolis(&k_q1, &k_q2, &k_q3); // margolis (Toffoli simplified)
    if (ret)
        return ret;

    log_symbolic_event("PERES", k_q1.id, k_q1.tag, "Peres gate applied");

    if (copy_to_user(user_q3, &k_q3, sizeof(k_q3)))
        return -EFAULT;

    return 0;
}

#endif
