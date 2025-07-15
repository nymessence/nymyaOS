// src/nymya_3348_hex_rhombi_lattice.c

#include <stdint.h>
#include <complex.h>
#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
#endif

#ifndef __KERNEL__

int nymya_3348_hex_rhombi_lattice(nymya_qubit* q[7]) {
    if (!q[0]) return -1;
    for (int i = 1; i < 7; i++) {
        if (!q[i]) return -1;
        hadamard(q[i]);
        cnot(q[0], q[i]); // entangle center with each outer
    }

    // Create rhombi (q[1]-q[2]-q[0]-q[6]), (q[2]-q[3]-q[0]-q[1]), ...
    for (int i = 1; i < 6; i++) {
        cnot(q[i], q[i+1]);
        cnot(q[i+1], q[0]);
    }
    cnot(q[6], q[1]);
    cnot(q[1], q[0]);

    log_symbolic_event("HEX_RHOMBI", q[0]->id, q[0]->tag, "Hexagon tessellated into 3 rhombi");
    return 0;
}

#else

SYSCALL_DEFINE1(nymya_3348_hex_rhombi_lattice, struct nymya_qubit __user * __user * , user_q_array) {
    struct nymya_qubit *k_q[7];

    if (!user_q_array)
        return -EINVAL;

    for (int i = 0; i < 7; i++) {
        k_q[i] = kmalloc(sizeof(struct nymya_qubit), GFP_KERNEL);
        if (!k_q[i]) {
            for (int j = 0; j < i; j++) kfree(k_q[j]);
            return -ENOMEM;
        }
    }

    for (int i = 0; i < 7; i++) {
        if (copy_from_user(k_q[i], user_q_array + i, sizeof(struct nymya_qubit))) {
            for (int j = 0; j <= i; j++) kfree(k_q[j]);
            return -EFAULT;
        }
    }

    int ret = nymya_3348_hex_rhombi_lattice(k_q);

    if (ret == 0) {
        for (int i = 0; i < 7; i++) {
            if (copy_to_user(user_q_array + i, k_q[i], sizeof(struct nymya_qubit))) {
                ret = -EFAULT;
                break;
            }
        }
    }

    for (int i = 0; i < 7; i++) kfree(k_q[i]);

    return ret;
}

#endif
