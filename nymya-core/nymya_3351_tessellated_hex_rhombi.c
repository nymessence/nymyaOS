// src/nymya_3351_tessellated_hex_rhombi.c

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

int nymya_3351_tessellated_hex_rhombi(nymya_qubit* q[], size_t count) {
    if (!q || count < 7) return -1;
    size_t groups = count / 7;
    for (size_t g = 0; g < groups; g++) {
        size_t b = 7*g;
        if (!q[b]) return -1;
        // entangle center with all six
        for (int i = 1; i <= 6; i++) {
            if (!q[b+i]) return -1;
            hadamard(q[b+i]);
            cnot(q[b], q[b+i]);
        }
        // build rhombi edges
        for (int i = 1; i <= 5; i++) {
            cnot(q[b+i], q[b+i+1]);
            cnot(q[b+i+1], q[b]);
        }
        cnot(q[b+6], q[b+1]);
        cnot(q[b+1], q[b]);
        log_symbolic_event("HEX_RHOM_T", q[b]->id, q[b]->tag, "Hex→3 rhombi tessellate");
    }
    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3351_tessellated_hex_rhombi,
    struct nymya_qubit __user * __user * , user_q_array,
    size_t, count) {

    if (!user_q_array || count < 7)
        return -EINVAL;

    size_t groups = count / 7;

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
        size_t b = 7 * g;
        if (!k_q[b]) {
            ret = -EINVAL;
            goto cleanup;
        }
        // entangle center with all six
        for (int i = 1; i <= 6; i++) {
            if (!k_q[b + i]) {
                ret = -EINVAL;
                goto cleanup;
            }
            hadamard(k_q[b + i]);
            cnot(k_q[b], k_q[b + i]);
        }
        // build rhombi edges
        for (int i = 1; i <= 5; i++) {
            cnot(k_q[b + i], k_q[b + i + 1]);
            cnot(k_q[b + i + 1], k_q[b]);
        }
        cnot(k_q[b + 6], k_q[b + 1]);
        cnot(k_q[b + 1], k_q[b]);
        log_symbolic_event("HEX_RHOM_T", k_q[b]->id, k_q[b]->tag, "Hex→3 rhombi tessellate");
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
