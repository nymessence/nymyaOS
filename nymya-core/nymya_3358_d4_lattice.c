// src/nymya_3358_d4_lattice.c 

#include <stdlib.h>
#include <math.h>
#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/slab.h>
#endif

#ifndef __KERNEL__

double dist4d(nymya_qpos4d a, nymya_qpos4d b) {
    return sqrt(pow(a.x - b.x, 2) +
                pow(a.y - b.y, 2) +
                pow(a.z - b.z, 2) +
                pow(a.w - b.w, 2));
}

int nymya_3358_d4_lattice(nymya_qpos4d q[], size_t count) {
    if (!q || count < 24) return -1;

    const double epsilon = 1.01;

    for (size_t i = 0; i < count; i++) {
        if (!q[i].q) return -1;
        hadamard(q[i].q);
    }

    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            if (dist4d(q[i], q[j]) <= epsilon) {
                cnot(q[i].q, q[j].q);
            }
        }
    }

    log_symbolic_event("D4_LATTICE", q[0].q->id, q[0].q->tag, "D4 lattice entangled in 4D");
    return 0;
}

#else

static double dist4d_k(const nymya_qpos4d *a, const nymya_qpos4d *b) {
    return sqrt(pow(a->x - b->x, 2) +
                pow(a->y - b->y, 2) +
                pow(a->z - b->z, 2) +
                pow(a->w - b->w, 2));
}

SYSCALL_DEFINE2(nymya_3358_d4_lattice,
    struct nymya_qpos4d __user *, user_q,
    size_t, count) {

    if (!user_q || count < 24)
        return -EINVAL;

    nymya_qpos4d *k_q = kmalloc_array(count, sizeof(nymya_qpos4d), GFP_KERNEL);
    if (!k_q)
        return -ENOMEM;

    if (copy_from_user(k_q, user_q, count * sizeof(nymya_qpos4d))) {
        kfree(k_q);
        return -EFAULT;
    }

    for (size_t i = 0; i < count; i++) {
        if (!k_q[i].q) {
            kfree(k_q);
            return -EINVAL;
        }
    }

    for (size_t i = 0; i < count; i++) {
        hadamard(k_q[i].q);
    }

    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            if (dist4d_k(&k_q[i], &k_q[j]) <= 1.01) {
                cnot(k_q[i].q, k_q[j].q);
            }
        }
    }

    log_symbolic_event("D4_LATTICE", k_q[0].q->id, k_q[0].q->tag, "D4 lattice entangled in 4D");

    if (copy_to_user(user_q, k_q, count * sizeof(nymya_qpos4d))) {
        kfree(k_q);
        return -EFAULT;
    }

    kfree(k_q);
    return 0;
}

#endif
