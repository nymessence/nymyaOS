// src/nymya_3329_fredkin.c

#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
#endif

#ifndef __KERNEL__

int nymya_3329_fredkin(nymya_qubit* q_ctrl, nymya_qubit* q1, nymya_qubit* q2) {
    if (!q_ctrl || !q1 || !q2) return -1;

    if (cabs(q_ctrl->amplitude) > 0.5) {
        _Complex double temp = q1->amplitude;
        q1->amplitude = q2->amplitude;
        q2->amplitude = temp;

        log_symbolic_event("FREDKIN", q1->id, q1->tag, "Control triggered SWAP");
    } else {
        log_symbolic_event("FREDKIN", q1->id, q1->tag, "Control = 0, no action");
    }

    return 0;
}

#else

SYSCALL_DEFINE3(nymya_3329_fredkin,
    struct nymya_qubit __user *, user_q_ctrl,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q_ctrl, k_q1, k_q2;

    if (!user_q_ctrl || !user_q1 || !user_q2)
        return -EINVAL;

    if (copy_from_user(&k_q_ctrl, user_q_ctrl, sizeof(k_q_ctrl)))
        return -EFAULT;

    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;

    if (cabs(k_q_ctrl.amplitude) > 0.5) {
        _Complex double temp = k_q1.amplitude;
        k_q1.amplitude = k_q2.amplitude;
        k_q2.amplitude = temp;

        log_symbolic_event("FREDKIN", k_q1.id, k_q1.tag, "Control triggered SWAP");
    } else {
        log_symbolic_event("FREDKIN", k_q1.id, k_q1.tag, "Control = 0, no action");
    }

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return 0;
}

#endif
