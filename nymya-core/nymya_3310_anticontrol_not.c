// src/nymya_3310_anticontrol_not.c
//
// Implements the Anti-Controlled-NOT (ACNOT) gate for Nymya qubits.
// Flips the target qubit's phase if the control qubit's amplitude magnitude < 0.5.
//
// User-space uses standard complex doubles.
// Kernel-space uses fixed-point math and safe user memory handling.
//

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_nymya_3310_anticontrol_not NYMYA_ACNOT_CODE

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
 * Function: nymya_3310_anticontrol_not
 * ------------------------------------
 * Applies the Anti-Controlled-NOT gate.
 * If control qubit's amplitude magnitude < 0.5, flips the sign of the target's amplitude.
 *
 * Parameters:
 *   q_ctrl   - pointer to control qubit
 *   q_target - pointer to target qubit
 *
 * Returns:
 *   0 on success, -1 if either pointer is NULL
 */
int nymya_3310_anticontrol_not(nymya_qubit* q_ctrl, nymya_qubit* q_target) {
    if (!q_ctrl || !q_target) return -1;

    double magnitude = cabs(q_ctrl->amplitude);

    if (magnitude < 0.5) {
        q_target->amplitude *= -1;
        log_symbolic_event("ACNOT", q_target->id, q_target->tag, "NOT via anti-control (ctrl=0)");
    } else {
        log_symbolic_event("ACNOT", q_target->id, q_target->tag, "No action (control = 1)");
    }

    return 0;
}

#else

/*
 * Syscall: nymya_3310_anticontrol_not
 * -----------------------------------
 * Kernel-space syscall handler for Anti-Controlled-NOT gate.
 * Checks control qubit magnitude; flips target amplitude sign if control is 'off'.
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
SYSCALL_DEFINE2(nymya_3310_anticontrol_not, 
    struct nymya_qubit __user *, user_ctrl, 
    struct nymya_qubit __user *, user_target) {

    struct nymya_qubit k_ctrl, k_target;
    int64_t ctrl_re, ctrl_im;
    __uint128_t mag_sq;
    const __uint128_t threshold = (__uint128_t)(0.5 * FIXED_POINT_SCALE) * (0.5 * FIXED_POINT_SCALE);

    if (!user_ctrl || !user_target)
        return -EINVAL;

    if (copy_from_user(&k_ctrl, user_ctrl, sizeof(k_ctrl)))
        return -EFAULT;
    if (copy_from_user(&k_target, user_target, sizeof(k_target)))
        return -EFAULT;

    ctrl_re = k_ctrl.amplitude.re;
    ctrl_im = k_ctrl.amplitude.im;

    // Compute magnitude squared in fixed-point
    mag_sq = (__uint128_t)ctrl_re * ctrl_re + (__uint128_t)ctrl_im * ctrl_im;

    if (mag_sq < threshold) {
        // Flip target amplitude sign (multiply by -1)
        k_target.amplitude.re = -k_target.amplitude.re;
        k_target.amplitude.im = -k_target.amplitude.im;

        log_symbolic_event("ACNOT", k_target.id, k_target.tag, "NOT via anti-control (ctrl=0)");
    } else {
        log_symbolic_event("ACNOT", k_target.id, k_target.tag, "No action (control = 1)");
    }

    if (copy_to_user(user_target, &k_target, sizeof(k_target)))
        return -EFAULT;

    return 0;
}

#endif

