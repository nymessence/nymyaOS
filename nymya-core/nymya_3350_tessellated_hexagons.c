// src/nymya_3350_tessellated_hexagons.c

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

int nymya_3350_tessellated_hexagons(nymya_qubit* q[], size_t count) {
    if (!q || count < 6) return -1;
    size_t groups = count / 6;
    for (size_t g = 0; g < groups; g++) {
        size_t base = 6*g;
        for (int i = 0; i < 6; i++) {
            if (!q[base + i]) return -1;
            hadamard(q[base + i]);
        }
        for (int i = 0; i < 6; i++) {
            cnot(q[base + i], q[base + (i+1)%6]);
        }
        log_symbolic_event("HEX_TESS", q[base]->id, q[base]->tag, "Hexagon ring entangle");
    }
    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3350_tessellated_hexagons,
    struct nymya_qubit __user * __user * , user_q_array,
    size_t, count) {

    if (!user_q_array || count < 6)
        return -EINVAL;

    size_t groups = count / 6;
    struct nymya_qubit **k_q = kmalloc_array(count, sizeof(struct nymya_qubit*), GFP_KERNEL);
    if (!k_q)
        return -ENOMEM;

    for (size_t i = 0; i < count; i++) {
        k_q[i] = kmalloc(sizeof(struct nymya_qubit), GFP_KERNEL);
        if (!k_q[i]) {
            for (size_t j = 0; j < i; j++) kfree(k_q[j]);
            kfree(k_q);
            return -ENOMEM;
        }
    }

    for (size_t i = 0; i < count; i++) {
        struct nymya_qubit temp;
        if (copy_from_user(&temp, *(user_q_array + i), sizeof(struct nymya_qubit))) {
            for (size_t j = 0; j < count; j++) kfree(k_q[j]);
            kfree(k_q);
            return -EFAULT;
        }
        *k_q[i] = temp;
    }

    int ret = 0;
    for (size_t g = 0; g < groups; g++) {
        size_t base = 6 * g;
        for (int i = 0; i < 6; i++) {
            if (!k_q[base + i]) {
                ret = -EINVAL;
                goto cleanup;
            }
            hadamard(k_q[base + i]);
        }
        for (int i = 0; i < 6; i++) {
            cnot(k_q[base + i], k_q[base + (i+1) % 6]);
        }
        log_symbolic_event("HEX_TESS", k_q[base]->id, k_q[base]->tag, "Hexagon ring entangle");
    }

    for (size_t i = 0; i < count; i++) {
        if (copy_to_user(*(user_q_array + i), k_q[i], sizeof(struct nymya_qubit))) {
            ret = -EFAULT;
            break;
        }
    }

cleanup:
    for (size_t i = 0; i < count; i++)
        kfree(k_q[i]);
    kfree(k_q);

    return ret;
}

#endif
