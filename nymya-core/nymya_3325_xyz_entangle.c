// src/nymya_3325_xyz_entangle.c

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

int nymya_3325_xyz_entangle(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    if (!q1 || !q2) return -1;

    _Complex double rot = cexp(I * theta);
    q1->amplitude *= rot;
    q2->amplitude *= conj(rot);

    log_symbolic_event("XYZ", q1->id, q1->tag, "Full XX+YY+ZZ entanglement");
    return 0;
}

#else

SYSCALL_DEFINE3(nymya_3325_xyz_entangle,
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

    _Complex double rot = cos(theta) + I * sin(theta);
    k_q1.amplitude *= rot;
    k_q2.amplitude *= conj(rot);

    log_symbolic_event("XYZ", k_q1.id, k_q1.tag, "Full XX+YY+ZZ entanglement");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return 0;
}

#endif
