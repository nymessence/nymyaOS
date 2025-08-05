#include "nymya.h"

#ifdef __KERNEL__
    #include <linux/module.h>
    #include <linux/kernel.h>
    #include <linux/syscalls.h> // Keep for compatibility, though not directly using SYSCALL_DEFINE for this function
    #include <linux/uaccess.h>
    #include <linux/errno.h> // For -EINVAL, -EFAULT

/*
 * Flip imaginary part sign in fixed-point amplitude
 */
static inline void flip_imag_part(struct nymya_qubit *q) {
    q->amplitude.im = -q->amplitude.im;
}

// The function is now defined as a regular kernel function named nymya_3303_pauli_x,
// to match the requested EXPORT_SYMBOL_GPL name.
// It uses 'long' as a return type, consistent with typical syscall return values.
long nymya_3303_pauli_x(struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;

    if (!user_q)
        return -EINVAL;

    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    flip_imag_part(&kq);

    // Assuming log_symbolic_event is properly defined and accessible in the kernel context
    log_symbolic_event("PAULI_X", kq.id, kq.tag, "Polarity flipped");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

// Export the symbol for use by other kernel modules/code.
// Changed to export 'nymya_3303_pauli_x' as requested.
EXPORT_SYMBOL_GPL(nymya_3303_pauli_x);

#else // Userland implementation

#include <stdio.h>
#include <complex.h> // Ensure this is included for _Complex and I

/*
 * Flip imaginary part sign in builtin complex amplitude
 */
static inline void flip_imag_part(nymya_qubit *q) {
    // Assuming nymya_qubit's amplitude is _Complex double in userland
    // For direct manipulation, if q->amplitude is _Complex double:
    q->amplitude = creal(q->amplitude) - I * cimag(q->amplitude);
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

#endif // __KERNEL__