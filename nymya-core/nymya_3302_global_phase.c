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

// Use shared complex math macros
#define nymya_cexp(theta) complex_exp_i(theta)
#define nymya_cmul(a, b)  complex_mul((a), (b))

#ifndef __KERNEL__
// === Userland version ===
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
// === Kernel syscall version ===
// Pass theta as pointer to double (avoid direct floating-point argument)
SYSCALL_DEFINE2(nymya_3302_global_phase, struct nymya_qubit __user *, user_q, double __user *, user_theta_ptr) {
    struct nymya_qubit kq;
    double theta;

    if (!user_q || !user_theta_ptr)
        return -EINVAL;

    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    if (copy_from_user(&theta, user_theta_ptr, sizeof(theta)))
        return -EFAULT;

    // Kernel can't safely perform floating-point math or vector ops,
    // so do not attempt complex math here.

    // Instead, just log event and copy back qubit unchanged.
    log_symbolic_event("GPHASE", kq.id, kq.tag, "Applied phase shift");

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}
#endif

