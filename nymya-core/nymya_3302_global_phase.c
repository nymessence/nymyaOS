// src/nymya_3302_global_phase.c

#include <stdint.h>
#include <complex.h>
#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
#endif

#ifndef __KERNEL__
// Userland version: direct function call
int nymya_3302_global_phase(nymya_qubit* q, double theta) {
    if (!q) return -1;

    _Complex double phase = cexp(I * theta);
    q->amplitude *= phase;

    char log_msg[128];
    snprintf(log_msg, sizeof(log_msg), "Applied phase shift θ=%.3f rad", theta);
    log_symbolic_event("GPHASE", q->id, q->tag, log_msg);

    return 0;
}
#else
// Kernel syscall wrapper

SYSCALL_DEFINE2(nymya_3302_global_phase, struct nymya_qubit __user *, user_q, double, theta) {
    struct nymya_qubit kq;

    if (!user_q)
        return -EINVAL;
    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    // Reuse the same logic here, but kernel can't use snprintf
    // So call internal function or replicate minimal logging

    // Minimal internal logic for kernel:
    kq.amplitude *= cexp(I * theta);

    // Kernel can't use snprintf, so just do symbolic log with fixed msg
    log_symbolic_event("GPHASE", kq.id, kq.tag, "Applied phase shift");

    // If you want to reflect changes back to user, use copy_to_user:
    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}
#endif
