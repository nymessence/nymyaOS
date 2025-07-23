// src/nymya_3308_hadamard_gate.c
//
// Implements the Hadamard gate (H gate) for a qubit amplitude.
// The Hadamard gate creates superposition by scaling the amplitude by 1/√2.
//
// In user-space, uses standard complex double arithmetic.
// In kernel-space, uses fixed-point arithmetic consistent with nymya.h definitions.
//

#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
#endif

#ifndef __KERNEL__

/*
 * Function: nymya_3308_hadamard_gate
 * ----------------------------------
 * Applies the Hadamard gate to the qubit's amplitude.
 * Scales the amplitude by 1/√2 (approximately 0.70710678).
 *
 * Parameters:
 *   q - pointer to nymya_qubit struct in user-space
 *
 * Returns:
 *   0 on success, -1 if q is NULL
 */
int nymya_3308_hadamard_gate(nymya_qubit* q) {
    if (!q) return -1;

    // Scale the complex amplitude by 1/sqrt(2)
    q->amplitude *= (1.0 / sqrt(2.0));

    log_symbolic_event("HADAMARD", q->id, q->tag, "Applied H gate (superposition)");
    return 0;
}

#else

/*
 * Syscall: nymya_3308_hadamard_gate
 * ---------------------------------
 * Kernel-space syscall handler for Hadamard gate.
 * Multiplies the qubit's amplitude by fixed-point 1/√2 using 64-bit integer math.
 *
 * Parameters:
 *   user_q - pointer to user-space nymya_qubit struct
 *
 * Returns:
 *   0 on success
 *   -EINVAL if user_q is NULL
 *   -EFAULT if copying from/to user space fails
 */
SYSCALL_DEFINE1(nymya_3308_hadamard_gate, struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;
    int64_t re, im;
    // Fixed-point representation of 1/sqrt(2)
    const int64_t scale = (int64_t)(0.70710678 * FIXED_POINT_SCALE);

    if (!user_q)
        return -EINVAL;
    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    re = kq.amplitude.re;
    im = kq.amplitude.im;

    // Multiply real and imaginary parts by scale, then shift back to fixed-point range
    kq.amplitude.re = (re * scale) >> 32;
    kq.amplitude.im = (im * scale) >> 32;

    log_symbolic_event("HADAMARD", kq.id, kq.tag, "Applied H gate (superposition)");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif

