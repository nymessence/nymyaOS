// src/nymya_3331_barenco.c

#include <stdlib.h>
#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
#endif

#ifndef __KERNEL__

// Barenco-style 3-qubit composite (H–CNOT–S–CNOT–H)
int nymya_3331_barenco(nymya_qubit* q1, nymya_qubit* q2, nymya_qubit* q3) {
    if (!q1 || !q2 || !q3) return -1;

    nymya_3308_hadamard_gate(q3);
    nymya_3309_controlled_not(q2, q3);
    nymya_3306_phase_gate(q3);
    nymya_3309_controlled_not(q1, q3);
    nymya_3308_hadamard_gate(q3);

    log_symbolic_event("BARENCO", q1->id, q1->tag, "Barenco composite applied");
    return 0;
}

#else

SYSCALL_DEFINE3(nymya_3331_barenco,
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

    int ret = nymya_3331_barenco(&k_q1, &k_q2, &k_q3);

    if (ret == 0)
        log_symbolic_event("BARENCO", k_q1.id, k_q1.tag, "Barenco composite applied");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    if (copy_to_user(user_q3, &k_q3, sizeof(k_q3)))
        return -EFAULT;

    return ret;
}

#endif
