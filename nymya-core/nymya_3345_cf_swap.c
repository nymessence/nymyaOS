// src/nymya_3345_cf_swap.c

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

int nymya_3345_cf_swap(nymya_qubit* qc, nymya_qubit* q1, nymya_qubit* q2) {
    if (!qc || !q1 || !q2) return -1;

    if (cabs(qc->amplitude) > 0.5) {
        nymya_3337_fermion_sim(q1, q2);
        log_symbolic_event("CF_SWAP", q1->id, q1->tag, "Controlled Fermionic SWAP");
    } else {
        log_symbolic_event("CF_SWAP", q1->id, q1->tag, "Control=0, no action");
    }
    return 0;
}

#else

SYSCALL_DEFINE3(nymya_3345_cf_swap,
    struct nymya_qubit __user *, user_qc,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_qc, k_q1, k_q2;
    int ret = 0;

    if (!user_qc || !user_q1 || !user_q2)
        return -EINVAL;

    if (copy_from_user(&k_qc, user_qc, sizeof(k_qc)))
        return -EFAULT;
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;

    if (cabs(k_qc.amplitude) > 0.5) {
        ret = nymya_3337_fermion_sim(&k_q1, &k_q2);
        if (ret)
            return ret;

        log_symbolic_event("CF_SWAP", k_q1.id, k_q1.tag, "Controlled Fermionic SWAP");
    } else {
        log_symbolic_event("CF_SWAP", k_q1.id, k_q1.tag, "Control=0, no action");
    }

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return ret;
}

#endif
