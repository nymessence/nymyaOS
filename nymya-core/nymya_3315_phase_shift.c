// src/nymya_3315_phase_shift.c

#include <stdint.h>
#include <complex.h>
#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
#endif

#ifndef __KERNEL__

int nymya_3315_phase_shift(nymya_qubit* q, double theta) {
    if (!q) return -1;

    q->amplitude *= cexp(I * theta);

    log_symbolic_event("PHASE_SHIFT", q->id, q->tag, "Applied variable phase shift");
    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3315_phase_shift,
    struct nymya_qubit __user *, user_q,
    double, theta) {

    struct nymya_qubit k_q;

    if (!user_q)
        return -EINVAL;

    if (copy_from_user(&k_q, user_q, sizeof(k_q)))
        return -EFAULT;

    // Kernel C standard lib doesn’t have cexp, so use kernel’s complex workaround or inline.
    // For now, approximate e^(iθ) as cos(θ) + i sin(θ)

    double cos_theta = cos(theta);
    double sin_theta = sin(theta);

    _Complex double phase = cos_theta + sin_theta * I;

    k_q.amplitude *= phase;

    log_symbolic_event("PHASE_SHIFT", k_q.id, k_q.tag, "Applied variable phase shift");

    if (copy_to_user(user_q, &k_q, sizeof(k_q)))
        return -EFAULT;

    return 0;
}

#endif
