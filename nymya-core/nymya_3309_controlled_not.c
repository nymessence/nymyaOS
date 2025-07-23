// src/nymya_3309_controlled_not.c

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

int nymya_3309_controlled_not(nymya_qubit* q_ctrl, nymya_qubit* q_target) {
    if (!q_ctrl || !q_target) return -1;

    if (cabs(q_ctrl->amplitude) > 0.5) {
        q_target->amplitude *= -1;
        log_symbolic_event("CNOT", q_target->id, q_target->tag, "NOT applied via control");
    } else {
        log_symbolic_event("CNOT", q_target->id, q_target->tag, "No action (control = 0)");
    }

    return 0;
}

#else

SYSCALL_DEFINE2(nymya_3309_controlled_not, 
    struct nymya_qubit __user *, user_ctrl, 
    struct nymya_qubit __user *, user_target) {

    struct nymya_qubit k_ctrl, k_target;
    double ctrl_real, ctrl_imag;
    double ctrl_magnitude;

    if (!user_ctrl || !user_target)
        return -EINVAL;

    if (copy_from_user(&k_ctrl, user_ctrl, sizeof(k_ctrl)))
        return -EFAULT;
    if (copy_from_user(&k_target, user_target, sizeof(k_target)))
        return -EFAULT;

    ctrl_real = __real k_ctrl.amplitude;
    ctrl_imag = __imag k_ctrl.amplitude;
    ctrl_magnitude = sqrt(ctrl_real * ctrl_real + ctrl_imag * ctrl_imag);

    if (ctrl_magnitude > 0.5) {
        // Flip target amplitude phase
        k_target.amplitude = (complex_double){ .real = -__real k_target.amplitude, .imag = -__imag k_target.amplitude };
        log_symbolic_event("CNOT", k_target.id, k_target.tag, "NOT applied via control");
    } else {
        log_symbolic_event("CNOT", k_target.id, k_target.tag, "No action (control = 0)");
    }

    if (copy_to_user(user_target, &k_target, sizeof(k_target)))
        return -EFAULT;

    return 0;
}

#endif

