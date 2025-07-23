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

#define nymya_cexp(theta) complex_exp_i(theta)
#define nymya_cmul(a, b)  complex_mul((a), (b))

/**
 * nymya_3302_global_phase - Apply global phase shift to a symbolic qubit
 * @q: Pointer to a symbolic qubit
 * @theta: Phase angle in radians
 *
 * Multiplies the qubit’s amplitude by e^(iθ), applying a global phase.
 * No probability amplitudes are altered, only the phase.
 * Logs symbolic event with phase shift details.
 *
 * Returns 0 on success, or -1 if the input is invalid.
 */
#ifndef __KERNEL__

int nymya_3302_global_phase(nymya_qubit* q, double theta) {
    if (!q) return -1;

    complex_double phase = nymya_cexp(theta);
    q->amplitude = nymya_cmul(q->amplitude, phase);

    char log_msg[128];
    snprintf(log_msg, sizeof(log_msg), "Applied phase shift θ=%.3f rad", theta);
    log_symbolic_event("GPHASE", q->id, q->tag, log_msg);

    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3302_global_phase, struct nymya_qubit __user *, user_q, double __user *, user_theta_ptr) {
    struct nymya_qubit kq;
    double theta;

    if (!user_q || !user_theta_ptr)
        return -EINVAL;

    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    if (copy_from_user(&theta, user_theta_ptr, sizeof(theta)))
        return -EFAULT;

    log_symbolic_event("GPHASE", kq.id, kq.tag, "Applied phase shift");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif

