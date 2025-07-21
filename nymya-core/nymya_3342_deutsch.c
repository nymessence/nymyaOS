// src/nymya_3342_deutsch.c

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

int nymya_3342_deutsch(nymya_qubit* q1, nymya_qubit* q2, void (*f)(nymya_qubit*)) {
    if (!q1 || !q2 || !f) return -1;
    hadamard(q1);
    f(q2);
    hadamard(q1);
    log_symbolic_event("DEUTSCH", q1->id, q1->tag, "Deutsch gate applied");
    return 0;
}

#else

// Note: Passing a function pointer from userspace to kernel isn’t practical,
// so here we assume 'f' is one of predefined gates known in kernel mode by a code.

SYSCALL_DEFINE3(nymya_3342_deutsch,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    int, func_code) {  // identify which function f to apply by an int code

    struct nymya_qubit k_q1, k_q2;
    int ret;

    if (!user_q1 || !user_q2)
        return -EINVAL;

    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;

    hadamard(&k_q1);

    switch (func_code) {
        case 0:  // example: phase_s
            ret = nymya_3306_phase_gate(&k_q2);
            break;
        case 1:  // example: sqrt_x
            ret = nymya_3307_sqrt_x_gate(&k_q2);
            break;
        // Add more cases for different gates as needed
        default:
            return -EINVAL;
    }

    hadamard(&k_q1);

    log_symbolic_event("DEUTSCH", k_q1.id, k_q1.tag, "Deutsch gate applied");

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return ret;
}

#endif
