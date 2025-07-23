// src/nymya_3308_hadamard_gate.c

#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
#endif

#ifndef __KERNEL__

int nymya_3308_hadamard_gate(nymya_qubit* q) {
    if (!q) return -1;

    q->amplitude *= (1.0 / sqrt(2.0));

    log_symbolic_event("HADAMARD", q->id, q->tag, "Applied H gate (superposition)");
    return 0;
}

#else

SYSCALL_DEFINE1(nymya_3308_hadamard_gate, struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;
    double real, imag;
    const double scale = 0.70710678; // 1/sqrt(2)

    if (!user_q)
        return -EINVAL;
    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    real = __real kq.amplitude;
    imag = __imag kq.amplitude;

    // Scale both real and imaginary parts
    kq.amplitude = (complex_double){ .real = real * scale, .imag = imag * scale };

    log_symbolic_event("HADAMARD", kq.id, kq.tag, "Applied H gate (superposition)");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif

