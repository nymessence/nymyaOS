// src/nymya_3305_pauli_z.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#define __NR_nymya_3305_pauli_z NYMYA_PAULI_Z_CODE

#include <unistd.h>
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
 * User-space implementation of Pauli-Z gate.
 * Multiplies the qubit amplitude by -1 (negates both real and imag parts).
 */
int nymya_3305_pauli_z(nymya_qubit* q) {
    if (!q) return -1;

    q->amplitude = make_complex(-complex_re(q->amplitude), -complex_im(q->amplitude));

    log_symbolic_event("PAULI_Z", q->id, q->tag, "Inverted inner state");
    return 0;
}

#else

/*
 * Kernel syscall: nymya_3305_pauli_z
 * Negates the fixed-point amplitude of the qubit.
 */
SYSCALL_DEFINE1(nymya_3305_pauli_z, struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;
    int64_t re, im;

    if (!user_q)
        return -EINVAL;
    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    re = kq.amplitude.re;
    im = kq.amplitude.im;

    kq.amplitude.re = -re;
    kq.amplitude.im = -im;

    log_symbolic_event("PAULI_Z", kq.id, kq.tag, "Inverted inner state");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif

