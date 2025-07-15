// src/nymya_3305_pauli_z.c

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

int nymya_3305_pauli_z(nymya_qubit* q) {
    if (!q) return -1;

    q->amplitude *= -1;

    log_symbolic_event("PAULI_Z", q->id, q->tag, "Inverted inner state");
    return 0;
}

#else

SYSCALL_DEFINE1(nymya_3305_pauli_z, struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;

    if (!user_q)
        return -EINVAL;
    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    kq.amplitude *= -1;

    log_symbolic_event("PAULI_Z", kq.id, kq.tag, "Inverted inner state");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif
