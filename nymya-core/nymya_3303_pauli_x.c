#include "nymya.h"

#ifdef __KERNEL__
    #include <linux/module.h>
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>

/*
 * Flip imaginary part sign in fixed-point amplitude
 */
static inline void flip_imag_part(struct nymya_qubit *q) {
    q->amplitude.im = -q->amplitude.im;
}

SYSCALL_DEFINE1(nymya_3303_pauli_x, struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;

    if (!user_q)
        return -EINVAL;

    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    flip_imag_part(&kq);

    log_symbolic_event("PAULI_X", kq.id, kq.tag, "Polarity flipped");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

// Export the symbol for use by other kernel modules/code
EXPORT_SYMBOL_GPL(nymya_3303_pauli_x);

#else

#include <stdio.h>

/*
 * Flip imaginary part sign in builtin complex amplitude
 */
static inline void flip_imag_part(nymya_qubit *q) {
    double re = complex_re(q->amplitude);
    double im = complex_im(q->amplitude);
    q->amplitude = make_complex(re, -im);
}

/*
 * Userland implementation of the Pauli-X gate
 * @q: pointer to symbolic qubit
 *
 * Flips the sign of the imaginary part of the amplitude.
 * Logs the symbolic event with qubit ID and tag.
 *
 * Returns 0 on success, -1 if input is null.
 */
int nymya_3303_pauli_x(nymya_qubit *q) {
    if (!q) return -1;

    flip_imag_part(q);

    log_symbolic_event("PAULI_X", q->id, q->tag, "Polarity flipped");
    return 0;
}

#endif

