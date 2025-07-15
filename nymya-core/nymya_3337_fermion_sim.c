// src/nymya_3337_fermion_sim.c

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

int nymya_3337_fermion_sim(nymya_qubit* q1, nymya_qubit* q2) {
    if (!q1 || !q2) return -1;

    swap(q1, q2);
    q1->amplitude *= -1;

    log_symbolic_event("FERMION_SIM", q1->id, q1->tag, "Fermionic exchange");
    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3337_fermion_sim,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2;

    if (!user_q1 || !user_q2)
        return -EINVAL;

    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;

    int ret = nymya_3337_fermion_sim(&k_q1, &k_q2);

    if (ret == 0)
        log_symbolic_event("FERMION_SIM", k_q1.id, k_q1.tag, "Fermionic exchange");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return ret;
}

#endif
