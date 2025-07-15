// src/nymya_3354_metatron_cube.c

#include <stdint.h>
#include <stdlib.h>
#include <complex.h>
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

int nymya_3354_metatron_cube(nymya_qubit* q[], size_t count) {
    if (!q || count < 13) return -1;

    for (size_t i = 0; i < 13; i++) {
        if (!q[i]) return -1;
        hadamard(q[i]);
    }

    for (size_t i = 1; i < 13; i++) {
        cnot(q[0], q[i]);
    }

    for (int i = 1; i <= 6; i++) {
        cnot(q[i], q[i + 6]);
    }

    log_symbolic_event("METATRON", q[0]->id, q[0]->tag,
        "Metatron’s Cube geometry entangled");
    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3354_metatron_cube,
    struct nymya_qubit __user * __user * user_q_array,
    size_t count) {

    if (!user_q_array || count < 13)
        return -EINVAL;

    struct nymya_qubit *k_q[13];

    for (int i = 0; i < 13; i++) {
        k_q[i] = kmalloc(sizeof(struct nymya_qubit), GFP_KERNEL);
        if (!k_q[i]) {
            for (int j = 0; j < i; j++) kfree(k_q[j]);
            return -ENOMEM;
        }
    }

    for (int i = 0; i < 13; i++) {
        struct nymya_qubit temp;
        if (copy_from_user(&temp, *(user_q_array + i), sizeof(struct nymya_qubit))) {
            for (int j = 0; j < 13; j++) kfree(k_q[j]);
            return -EFAULT;
        }
        *k_q[i] = temp;
    }

    for (int i = 0; i < 13; i++) {
        if (!k_q[i]) {
            for (int j = 0; j < 13; j++) kfree(k_q[j]);
            return -EINVAL;
        }
    }

    for (int i = 0; i < 13; i++) {
        hadamard(k_q[i]);
    }

    for (int i = 1; i < 13; i++) {
        cnot(k_q[0], k_q[i]);
    }

    for (int i = 1; i <= 6; i++) {
        cnot(k_q[i], k_q[i + 6]);
    }

    log_symbolic_event("METATRON", k_q[0]->id, k_q[0]->tag,
        "Metatron’s Cube geometry entangled");

    for (int i = 0; i < 13; i++) {
        if (copy_to_user(*(user_q_array + i), k_q[i], sizeof(struct nymya_qubit))) {
            for (int j = 0; j < 13; j++) kfree(k_q[j]);
            return -EFAULT;
        }
    }

    for (int i = 0; i < 13; i++) kfree(k_q[i]);

    return 0;
}

#endif
