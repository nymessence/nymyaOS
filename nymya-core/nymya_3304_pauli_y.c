// src/nymya_3304_pauli_y.c

#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
#endif

#ifndef __KERNEL__

int nymya_3304_pauli_y(nymya_qubit* q) {
    if (!q) return -1;

    q->amplitude *= I;

    log_symbolic_event("PAULI_Y", q->id, q->tag, "Dream vector rotated");
    return 0;
}

#else

SYSCALL_DEFINE1(nymya_3304_pauli_y, struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;
    double real, imag;

    if (!user_q)
        return -EINVAL;
    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    // Multiply amplitude by I: (a + bi) * i = -b + ai
    real = __real kq.amplitude;
    imag = __imag kq.amplitude;
    kq.amplitude = (complex_double){ .real = -imag, .imag = real };

    log_symbolic_event("PAULI_Y", kq.id, kq.tag, "Dream vector rotated");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif

