// src/nymya_3318_controlled_phase_s.c

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

int nymya_3318_controlled_phase_s(nymya_qubit* qc, nymya_qubit* qt) {
    if (!qc || !qt) return -1;

    if (cabs(qc->amplitude) > 0.5) {
        qt->amplitude *= cexp(I * (M_PI / 2.0));
        log_symbolic_event("C-PHASE-S", qt->id, qt->tag, "Conditional S phase applied");
    } else {
        log_symbolic_event("C-PHASE-S", qt->id, qt->tag, "No action (control = 0)");
    }

    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3318_controlled_phase_s,
    struct nymya_qubit __user *, user_qc,
    struct nymya_qubit __user *, user_qt) {

    struct nymya_qubit k_qc, k_qt;

    if (!user_qc || !user_qt)
        return -EINVAL;

    if (copy_from_user(&k_qc, user_qc, sizeof(k_qc)))
        return -EFAULT;
    if (copy_from_user(&k_qt, user_qt, sizeof(k_qt)))
        return -EFAULT;

    if (cabs(k_qc.amplitude) > 0.5) {
        double cos_val = 0.0;
        double sin_val = 1.0; // sin(π/2) = 1
        _Complex double phase = cos_val + sin_val * I;
        k_qt.amplitude *= phase;
        log_symbolic_event("C-PHASE-S", k_qt.id, k_qt.tag, "Conditional S phase applied");
    } else {
        log_symbolic_event("C-PHASE-S", k_qt.id, k_qt.tag, "No action (control = 0)");
    }

    if (copy_to_user(user_qt, &k_qt, sizeof(k_qt)))
        return -EFAULT;

    return 0;
}

#endif
