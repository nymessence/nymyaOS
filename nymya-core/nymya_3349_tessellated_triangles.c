// src/nymya_3349_tessellated_triangles.c

#include <stdlib.h>
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

int nymya_3349_tessellated_triangles(nymya_qubit* q[], size_t count) {
    if (!q || count < 3) return -1;
    size_t groups = count / 3;
    for (size_t g = 0; g < groups; g++) {
        nymya_qubit *a = q[3*g], *b = q[3*g+1], *c = q[3*g+2];
        if (!a || !b || !c) return -1;
        hadamard(a);
        cnot(a, b);
        cnot(b, c);
        cnot(c, a);
        log_symbolic_event("TRI_TESS", a->id, a->tag, "Triangle entangle");
    }
    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3349_tessellated_triangles,
    struct nymya_qubit __user * __user * , user_q_array,
    size_t, count) {

    if (!user_q_array || count < 3)
        return -EINVAL;

    size_t groups = count / 3;
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

    // Copy qubits from user space pointers
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
        if (!k_q[3*g] || !k_q[3*g + 1] || !k_q[3*g + 2]) {
            ret = -EINVAL;
            break;
        }
        hadamard(k_q[3*g]);
        cnot(k_q[3*g], k_q[3*g + 1]);
        cnot(k_q[3*g + 1], k_q[3*g + 2]);
        cnot(k_q[3*g + 2], k_q[3*g]);
        log_symbolic_event("TRI_TESS", k_q[3*g]->id, k_q[3*g]->tag, "Triangle entangle");
    }

    if (ret == 0) {
        for (size_t i = 0; i < count; i++) {
            if (copy_to_user(*(user_q_array + i), k_q[i], sizeof(struct nymya_qubit))) {
                ret = -EFAULT;
                break;
            }
        }
    }

    for (size_t i = 0; i < count; i++) kfree(k_q[i]);
    kfree(k_q);

    return ret;
}

#endif
