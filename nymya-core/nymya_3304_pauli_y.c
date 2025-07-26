// src/nymya_3304_pauli_y.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#define __NR_nymya_3304_pauli_y NYMYA_PAULI_Y_CODE

#include <unistd.h>
    #include <stdio.h>
    #include <stdlib.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/module.h> // Required for EXPORT_SYMBOL_GPL
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

/**
 * @brief Applies the Pauli-Y gate logic to a kernel-space qubit.
 *
 * This function performs the core operation of the Pauli-Y gate,
 * multiplying the qubit's amplitude by the imaginary unit 'i'.
 * In the fixed-point complex representation, this involves swapping
 * the real and imaginary parts and negating the new real part.
 *
 * @param kq A pointer to the kernel-space nymya_qubit structure to operate on.
 * @return 0 on success, or a negative error code if an internal issue occurs.
 */
int nymya_3304_pauli_y(struct nymya_qubit *kq) {
    int64_t re, im;

    // A defensive check, though kq should be valid from the syscall wrapper
    if (!kq) {
        return -EINVAL;
    }

    // Extract fixed-point real and imaginary parts
    re = kq->amplitude.re;
    im = kq->amplitude.im;

    // Multiply amplitude by i: (a + bi)*i = -b + ai
    kq->amplitude.re = -im;
    kq->amplitude.im = re;

    log_symbolic_event("PAULI_Y", kq->id, kq->tag, "Dream vector rotated");

    return 0; // Success
}
EXPORT_SYMBOL_GPL(nymya_3304_pauli_y);

/*
 * Kernel syscall: nymya_3304_pauli_y
 * Multiplies the qubit amplitude by i.
 * Note: kernel uses fixed-point complex, so manually swap real and imag parts.
 */
SYSCALL_DEFINE1(nymya_3304_pauli_y, struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;
    int ret;

    if (!user_q)
        return -EINVAL;
    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    // Call the extracted core logic function
    ret = nymya_3304_pauli_y(&kq);
    if (ret) {
        // Propagate any error from the core logic
        return ret;
    }

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif
