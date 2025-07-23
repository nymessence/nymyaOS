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

/*
 * User-space implementation of Pauli-Y gate on a qubit.
 * Multiplies amplitude by the imaginary unit i:
 * (a + bi) * i = -b + ai
 */
int nymya_3304_pauli_y(nymya_qubit* q) {
    if (!q) return -1;

    // Using complex_double as _Complex double in user-space:
    q->amplitude = make_complex(-complex_im(q->amplitude), complex_re(q->amplitude));

    log_symbolic_event("PAULI_Y", q->id, q->tag, "Dream vector rotated");
    return 0;
}

#else

/*
 * Kernel syscall: nymya_3304_pauli_y
 * Multiplies the qubit amplitude by i.
 * Note: kernel uses fixed-point complex, so manually swap real and imag parts.
 */
SYSCALL_DEFINE1(nymya_3304_pauli_y, struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;
    int64_t re, im;

    if (!user_q)
        return -EINVAL;
    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    // Extract fixed-point real and imaginary parts
    re = kq.amplitude.re;
    im = kq.amplitude.im;

    // Multiply amplitude by i: (a + bi)*i = -b + ai
    kq.amplitude.re = -im;
    kq.amplitude.im = re;

    log_symbolic_event("PAULI_Y", kq.id, kq.tag, "Dream vector rotated");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif

