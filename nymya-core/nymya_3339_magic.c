// src/nymya_3339_magic.c

#include <stdint.h>
#include <complex.h>
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

int nymya_3339_magic(nymya_qubit* q1, nymya_qubit* q2) {
    if (!q1 || !q2) return -1;

    hadamard(q1);
    phase_s(q1);
    cnot(q1, q2);
    hadamard(q1);

    log_symbolic_event("MAGIC", q1->id, q1->tag, "Magic gate applied");
    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3339_magic,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2;

    if (!user_q1 || !user_q2)
        return -EINVAL;

    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;

    int ret = nymya_3339_magic(&k_q1, &k_q2);

    if (ret == 0)
        log_symbolic_event("MAGIC", k_q1.id, k_q1.tag, "Magic gate applied");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return ret;
}

#endif
