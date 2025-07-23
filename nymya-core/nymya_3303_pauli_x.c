#include "nymya.h"

#ifdef __KERNEL__
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>

SYSCALL_DEFINE1(nymya_3303_pauli_x, struct nymya_qubit __user *, user_q) {
    struct nymya_qubit kq;

    if (!user_q)
        return -EINVAL;

    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    // Flip imaginary part sign: conjugate amplitude
    kq.amplitude.im = -kq.amplitude.im;

    log_symbolic_event("PAULI_X", kq.id, kq.tag, "Polarity flipped");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#else // User-space implementation

#include <stdio.h>

int nymya_3303_pauli_x(nymya_qubit* q) {
    if (!q) return -1;

    // Flip imaginary part sign: conjugate amplitude
    q->amplitude.im = -q->amplitude.im;

    log_symbolic_event("PAULI_X", q->id, q->tag, "Polarity flipped");
    return 0;
}

#endif

