// src/nymya_3347_hexagonal_lattice.c

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

int nymya_3347_hexagonal_lattice(nymya_qubit* q[6]) {
    for (int i = 0; i < 6; i++) {
        if (!q[i]) return -1;
        hadamard(q[i]);
    }
    for (int i = 0; i < 6; i++) {
        cnot(q[i], q[(i+1) % 6]);
    }

    log_symbolic_event("HEX_LATTICE", q[0]->id, q[0]->tag, "Hexagonal ring lattice formed");
    return 0;
}

#else

SYSCALL_DEFINE1(nymya_3347_hexagonal_lattice, struct nymya_qubit __user * __user * , user_q_array) {
    struct nymya_qubit *k_q[6];

    if (!user_q_array)
        return -EINVAL;

    // Allocate kernel array pointers
    for (int i = 0; i < 6; i++) {
        k_q[i] = kmalloc(sizeof(struct nymya_qubit), GFP_KERNEL);
        if (!k_q[i]) {
            for (int j = 0; j < i; j++) kfree(k_q[j]);
            return -ENOMEM;
        }
    }

    // Copy each qubit from user space
    for (int i = 0; i < 6; i++) {
        if (copy_from_user(k_q[i], user_q_array + i, sizeof(struct nymya_qubit))) {
            for (int j = 0; j <= i; j++) kfree(k_q[j]);
            return -EFAULT;
        }
    }

    int ret = nymya_3347_hexagonal_lattice(k_q);

    if (ret == 0) {
        // Copy back modified qubits to user space
        for (int i = 0; i < 6; i++) {
            if (copy_to_user(user_q_array + i, k_q[i], sizeof(struct nymya_qubit))) {
                ret = -EFAULT;
                break;
            }
        }
    }

    for (int i = 0; i < 6; i++) kfree(k_q[i]);

    return ret;
}

#endif
