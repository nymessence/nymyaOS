// src/nymya_3312_double_controlled_not.c

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

int nymya_3312_double_controlled_not(nymya_qubit* qc1, nymya_qubit* qc2, nymya_qubit* qt) {
    if (!qc1 || !qc2 || !qt) return -1;

    if (cabs(qc1->amplitude) > 0.5 && cabs(qc2->amplitude) > 0.5) {
        qt->amplitude *= -1;
        log_symbolic_event("DCNOT", qt->id, qt->tag, "Double control triggered NOT");
    } else {
        log_symbolic_event("DCNOT", qt->id, qt->tag, "Conditions not met");
    }

    return 0;
}

#else

SYSCALL_DEFINE3(nymya_3312_double_controlled_not,
    struct nymya_qubit __user *, user_qc1,
    struct nymya_qubit __user *, user_qc2,
    struct nymya_qubit __user *, user_qt) {

    struct nymya_qubit k_qc1, k_qc2, k_qt;

    if (!user_qc1 || !user_qc2 || !user_qt)
        return -EINVAL;

    if (copy_from_user(&k_qc1, user_qc1, sizeof(k_qc1)))
        return -EFAULT;
    if (copy_from_user(&k_qc2, user_qc2, sizeof(k_qc2)))
        return -EFAULT;
    if (copy_from_user(&k_qt, user_qt, sizeof(k_qt)))
        return -EFAULT;

    if (cabs(k_qc1.amplitude) > 0.5 && cabs(k_qc2.amplitude) > 0.5) {
        k_qt.amplitude *= -1;
        log_symbolic_event("DCNOT", k_qt.id, k_qt.tag, "Double control triggered NOT");
    } else {
        log_symbolic_event("DCNOT", k_qt.id, k_qt.tag, "Conditions not met");
    }

    if (copy_to_user(user_qt, &k_qt, sizeof(k_qt)))
        return -EFAULT;

    return 0;
}

#endif
