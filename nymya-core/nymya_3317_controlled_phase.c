// src/nymya_3317_controlled_phase.c

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

int nymya_3317_controlled_phase(nymya_qubit* qc, nymya_qubit* qt, double theta) {
    if (!qc || !qt) return -1;

    if (cabs(qc->amplitude) > 0.5) {
        qt->amplitude *= cexp(I * theta);
        log_symbolic_event("C-PHASE", qt->id, qt->tag, "Controlled phase applied");
    } else {
        log_symbolic_event("C-PHASE", qt->id, qt->tag, "No action (control = 0)");
    }

    return 0;
}

#else

SYSCALL_DEFINE3(nymya_3317_controlled_phase,
    struct nymya_qubit __user *, user_qc,
    struct nymya_qubit __user *, user_qt,
    double, theta) {

    struct nymya_qubit k_qc, k_qt;

    if (!user_qc || !user_qt)
        return -EINVAL;

    if (copy_from_user(&k_qc, user_qc, sizeof(k_qc)))
        return -EFAULT;
    if (copy_from_user(&k_qt, user_qt, sizeof(k_qt)))
        return -EFAULT;

    if (cabs(k_qc.amplitude) > 0.5) {
        double cos_theta = cos(theta);
        double sin_theta = sin(theta);
        _Complex double phase = cos_theta + sin_theta * I;
        k_qt.amplitude *= phase;
        log_symbolic_event("C-PHASE", k_qt.id, k_qt.tag, "Controlled phase applied");
    } else {
        log_symbolic_event("C-PHASE", k_qt.id, k_qt.tag, "No action (control = 0)");
    }

    if (copy_to_user(user_qt, &k_qt, sizeof(k_qt)))
        return -EFAULT;

    return 0;
}

#endif
