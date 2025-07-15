// src/nymya_3330_rotate.c

#include <stdint.h>
#include <complex.h>
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

int nymya_3330_rotate(nymya_qubit* q, char axis, double theta) {
    if (!q) return -1;

    switch (axis) {
        case 'X':
        case 'x':
            nymya_3319_rotate_x(q, theta);
            break;
        case 'Y':
        case 'y':
            nymya_3320_rotate_y(q, theta);
            break;
        case 'Z':
        case 'z':
            nymya_3321_rotate_z(q, theta);
            break;
        default:
            log_symbolic_event("ROTATE", q->id, q->tag, "Unknown axis");
            return -1;
    }

    log_symbolic_event("ROTATE", q->id, q->tag, "Axis rotation applied");
    return 0;
}

#else

SYSCALL_DEFINE3(nymya_3330_rotate,
    struct nymya_qubit __user *, user_q,
    char, axis,
    double, theta) {

    struct nymya_qubit k_q;

    if (!user_q)
        return -EINVAL;

    if (copy_from_user(&k_q, user_q, sizeof(k_q)))
        return -EFAULT;

    int ret = -1;
    switch (axis) {
        case 'X':
        case 'x':
            ret = nymya_3319_rotate_x(&k_q, theta);
            break;
        case 'Y':
        case 'y':
            ret = nymya_3320_rotate_y(&k_q, theta);
            break;
        case 'Z':
        case 'z':
            ret = nymya_3321_rotate_z(&k_q, theta);
            break;
        default:
            log_symbolic_event("ROTATE", k_q.id, k_q.tag, "Unknown axis");
            return -EINVAL;
    }

    if (ret == 0)
        log_symbolic_event("ROTATE", k_q.id, k_q.tag, "Axis rotation applied");

    if (copy_to_user(user_q, &k_q, sizeof(k_q)))
        return -EFAULT;

    return ret;
}

#endif
