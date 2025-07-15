// src/nymya_3303_pauli_x.c

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

int nymya_3303_pauli_x(nymya_qubit* q) {
    if (!q) return -1;

    _Complex double temp = q->amplitude;
    q->amplitude = creal(temp) - cimag(temp)*I;

    log_symbolic_event("PAULI_X", q->id, q->tag, "Polarity flipped");
    return 0;
}

#else

SYSCALL_DEFINE1(nymya_3303_pauli_x, struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;

    if (!user_q)
        return -EINVAL;
    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    // Kernel logic equivalent to userland logic:
    _Complex double temp = kq.amplitude;
    kq.amplitude = creal(temp) - cimag(temp)*I;

    log_symbolic_event("PAULI_X", kq.id, kq.tag, "Polarity flipped");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif
