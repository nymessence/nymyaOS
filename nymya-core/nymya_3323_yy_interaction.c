// src/nymya_3323_yy_interaction.c

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

int nymya_3323_yy_interaction(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    if (!q1 || !q2) return -1;

    _Complex double rot = cos(theta) + I * sin(theta);
    q1->amplitude *= rot;
    q2->amplitude *= conj(rot);  // inverse for symmetry

    log_symbolic_event("YY", q2->id, q2->tag, "Applied YY interaction");
    return 0;
}

#else

SYSCALL_DEFINE3(nymya_3323_yy_interaction,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    double, theta) {

    struct nymya_qubit k_q1, k_q2;

    if (!user_q1 || !user_q2)
        return -EINVAL;

    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;

    double cos_val = cos(theta);
    double sin_val = sin(theta);
    _Complex double rot = cos_val + I * sin_val;
    _Complex double inv_rot = cos_val - I * sin_val;

    k_q1.amplitude *= rot;
    k_q2.amplitude *= inv_rot;

    log_symbolic_event("YY", k_q2.id, k_q2.tag, "Applied YY interaction");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return 0;
}

#endif
