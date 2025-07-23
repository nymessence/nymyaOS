// src/nymya_3309_controlled_not.c
//
// Implements the Controlled-NOT (CNOT) gate for Nymya qubits.
// The CNOT flips the target qubit's phase if the control qubit's amplitude magnitude > 0.5.
//
// User-space uses standard complex doubles.
// Kernel-space uses fixed-point math and careful user memory handling.
//

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

#ifndef __KERNEL__

/*
 * Function: nymya_3309_controlled_not
 * -----------------------------------
 * Applies the Controlled-NOT gate.
 * If control qubit's amplitude magnitude > 0.5, flips the sign of the target's amplitude.
 *
 * Parameters:
 *   q_ctrl   - pointer to control qubit
 *   q_target - pointer to target qubit
 *
 * Returns:
 *   0 on success, -1 if either pointer is NULL
 */
int nymya_3309_controlled_not(nymya_qubit* q_ctrl, nymya_qubit* q_target) {
    if (!q_ctrl || !q_target) return -1;

    // Compute magnitude of control qubit amplitude
    double magnitude = cabs(q_ctrl->amplitude);

    if (magnitude > 0.5) {
        // Flip target amplitude phase
        q_target->amplitude *= -1;
        log_symbolic_event("CNOT", q_target->id, q_target->tag, "NOT applied via control");
    } else {
        log_symbolic_event("CNOT", q_target->id, q_target->tag, "No action (control = 0)");
    }

    return 0;
}

#else

/*
 * Syscall: nymya_3309_controlled_not
 * ----------------------------------
 * Kernel-space syscall handler for CNOT gate.
 * Checks control qubit magnitude; flips target amplitude sign if control is 'on'.
 *
 * Parameters:
 *   user_ctrl  - pointer to user-space control qubit
 *   user_target - pointer to user-space target qubit
 *
 * Returns:
 *   0 on success
 *   -EINVAL if either pointer is NULL
 *   -EFAULT if copying from/to user memory fails
 */
SYSCALL_DEFINE2(nymya_3309_controlled_not, 
    struct nymya_qubit __user *, user_ctrl, 
    struct nymya_qubit __user *, user_target) {

    struct nymya_qubit k_ctrl, k_target;
    int64_t re, im;
    int64_t ctrl_re, ctrl_im;
    uint64_t id;
    char tag[NYMYA_TAG_MAXLEN];
    uint64_t tag_len;

    if (!user_ctrl || !user_target)
        return -EINVAL;

    if (copy_from_user(&k_ctrl, user_ctrl, sizeof(k_ctrl)))
        return -EFAULT;
    if (copy_from_user(&k_target, user_target, sizeof(k_target)))
        return -EFAULT;

    ctrl_re = k_ctrl.amplitude.re;
    ctrl_im = k_ctrl.amplitude.im;

    // Calculate magnitude squared in fixed-point: (re^2 + im^2) >> 64 for scale
    __uint128_t mag_sq = (__uint128_t)ctrl_re * ctrl_re + (__uint128_t)ctrl_im * ctrl_im;

    // Threshold magnitude ~0.5 in fixed point squared
    const __uint128_t threshold = (__uint128_t)(0.5 * FIXED_POINT_SCALE) * (0.5 * FIXED_POINT_SCALE);

    if (mag_sq > threshold) {
        // Flip target amplitude sign (multiply by -1)
        k_target.amplitude.re = -k_target.amplitude.re;
        k_target.amplitude.im = -k_target.amplitude.im;

        log_symbolic_event("CNOT", k_target.id, k_target.tag, "NOT applied via control");
    } else {
        log_symbolic_event("CNOT", k_target.id, k_target.tag, "No action (control = 0)");
    }

    if (copy_to_user(user_target, &k_target, sizeof(k_target)))
        return -EFAULT;

    return 0;
}

#endif

