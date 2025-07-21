// src/nymya_3352_e8_group.c

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

int nymya_3352_e8_group(nymya_qubit* q[8]) {
    for (int i = 0; i < 8; i++) {
        if (!q[i]) return -1;
        hadamard(q[i]);
    }
    for (int i = 0; i < 8; i++) {
        for (int j = i + 1; j < 8; j++) {
            cnot(q[i], q[j]);
            cnot(q[j], q[i]);
        }
    }
    log_symbolic_event("E8_GROUP", q[0]->id, q[0]->tag, "E8 8-node full entanglement");
    return 0;
}

#else

SYSCALL_DEFINE1(nymya_3352_e8_group,
    struct nymya_qubit __user * __user * , user_q_array) {

    if (!user_q_array)
        return -EINVAL;

    struct nymya_qubit *k_q[8];

    // Allocate kernel copies for each qubit pointer
    for (int i = 0; i < 8; i++) {
        k_q[i] = kmalloc(sizeof(struct nymya_qubit), GFP_KERNEL);
        if (!k_q[i]) {
            for (int j = 0; j < i; j++) kfree(k_q[j]);
            return -ENOMEM;
        }
    }

    // Copy from user space pointers
    for (int i = 0; i < 8; i++) {
        struct nymya_qubit temp;
        if (copy_from_user(&temp, *(user_q_array + i), sizeof(struct nymya_qubit))) {
            for (int j = 0; j < 8; j++) kfree(k_q[j]);
            return -EFAULT;
        }
        *k_q[i] = temp;
    }

    // Check for null pointers inside kernel copies (amplitude pointer, etc.)
    for (int i = 0; i < 8; i++) {
        if (!k_q[i]) {
            for (int j = 0; j < 8; j++) kfree(k_q[j]);
            return -EINVAL;
        }
    }

    // Run the logic
    for (int i = 0; i < 8; i++) {
        hadamard(k_q[i]);
    }
    for (int i = 0; i < 8; i++) {
        for (int j = i + 1; j < 8; j++) {
            cnot(k_q[i], k_q[j]);
            cnot(k_q[j], k_q[i]);
        }
    }

    log_symbolic_event("E8_GROUP", k_q[0]->id, k_q[0]->tag, "E8 8-node full entanglement");

    // Copy results back to user space
    for (int i = 0; i < 8; i++) {
        if (copy_to_user(*(user_q_array + i), k_q[i], sizeof(struct nymya_qubit))) {
            for (int j = 0; j < 8; j++) kfree(k_q[j]);
            return -EFAULT;
        }
    }

    for (int i = 0; i < 8; i++) kfree(k_q[i]);

    return 0;
}

#endif
