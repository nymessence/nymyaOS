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
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_nymya_3308_hadamard_gate NYMYA_HADAMARD_CODE

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/module.h> // ADDED: Required for EXPORT_SYMBOL_GPL
#endif

#ifndef __KERNEL__

/*
 * Function: nymya_3308_hadamard_gate
 * ----------------------------------
 * Applies the Hadamard gate to the qubit's amplitude.
 * Scales the amplitude by 1/√2 (approximately 0.70710678).
 *
 * Parameters:
 * q - pointer to nymya_qubit struct in user-space
 *
 * Returns:
 * 0 on success, -1 if q is NULL
 */
int nymya_3308_hadamard_gate(nymya_qubit* q) {
    if (!q) return -1;

    // Scale the complex amplitude by 1/sqrt(2)
    q->amplitude *= (1.0 / sqrt(2.0));

    log_symbolic_event("HADAMARD", q->id, q->tag, "Applied H gate (superposition)");
    return 0;
}

#else // __KERNEL__

/**
 * nymya_3308_hadamard_gate - Core kernel function for Hadamard gate.
 * @q: Pointer to the nymya_qubit structure.
 *
 * This function applies the Hadamard gate logic, scaling the qubit's amplitude
 * by a fixed-point approximation of 1/√2.
 * This function is designed to be called directly by other kernel code.
 *
 * Returns:
 * 0 on success.
 * -EINVAL if the qubit pointer is NULL.
 */
int nymya_3308_hadamard_gate(struct nymya_qubit *q) {
    int64_t re, im;
    // Fixed-point representation of 1/sqrt(2)
    const int64_t scale = (int64_t)(0.70710678 * FIXED_POINT_SCALE);

    if (!q) {
        pr_err("NYMYA: nymya_3308_hadamard_gate received NULL qubit pointer\n");
        return -EINVAL;
    }

    re = q->amplitude.re;
    im = q->amplitude.im;

    // Multiply real and imaginary parts by scale, then shift back to fixed-point range
    // Assuming FIXED_POINT_SCALE is 2^32, so right shift by 32 bits.
    q->amplitude.re = (re * scale) >> 32;
    q->amplitude.im = (im * scale) >> 32;

    log_symbolic_event("HADAMARD", q->id, q->tag, "Applied H gate (superposition)");
    return 0;
}
// Export the symbol for this function so other kernel modules/code can call it directly.
EXPORT_SYMBOL_GPL(nymya_3308_hadamard_gate);


/*
 * Syscall: nymya_3308_hadamard_gate
 * ---------------------------------
 * Kernel-space syscall handler for Hadamard gate.
 * Multiplies the qubit's amplitude by fixed-point 1/√2 using 64-bit integer math.
 *
 * Parameters:
 * user_q - pointer to user-space nymya_qubit struct
 *
 * Returns:
 * 0 on success
 * -EINVAL if user_q is NULL
 * -EFAULT if copying from/to user space fails
 */
SYSCALL_DEFINE1(nymya_3308_hadamard_gate, struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;
    int ret;

    if (!user_q)
        return -EINVAL;
    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    // Call the core logic function defined above
    ret = nymya_3308_hadamard_gate(&kq);

    if (ret) // If the core function returned an error, propagate it
        return ret;

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif

