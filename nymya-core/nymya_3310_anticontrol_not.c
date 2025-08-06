// src/nymya_3310_anticontrol_not.c
//
// Implements the Anti-Controlled-NOT (ACNOT) gate for Nymya qubits.
// Flips the target qubit's phase if the control qubit's amplitude magnitude < 0.5.
//
// User-space uses standard complex doubles for qubit representation.
// Kernel-space uses fixed-point math and handles user memory safely.
//
// This file contains both userland syscall wrapper stub and kernel-side core logic.

#include "nymya.h"

#ifndef __KERNEL__

#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define __NR_nymya_3310_anticontrol_not NYMYA_ACNOT_CODE

/**
 * nymya_3310_anticontrol_not_user - Userland wrapper for ACNOT syscall.
 * @control: pointer to the control qubit (complex double).
 * @target: pointer to the target qubit (complex double).
 *
 * Returns 0 on success, -errno on failure.
 */
int nymya_3310_anticontrol_not_user(const void *control, void *target) {
    int ret = syscall(__NR_nymya_3310_anticontrol_not, control, target);
    if (ret < 0) {
        perror("nymya_3310_anticontrol_not syscall failed");
        return -ret;
    }
    return 0;
}

int nymya_3310_anticontrol_not(nymya_qubit* q_ctrl, nymya_qubit* q_target) {
    if (!q_ctrl || !q_target) return -1;

    double magnitude = cabs(q_ctrl->amplitude);

    if (magnitude < 0.5) {
        q_target->amplitude *= -1;
        log_symbolic_event("ACNOT", q_target->id, q_target->tag, "Phase flipped due to control");
    } else {
        log_symbolic_event("ACNOT", q_target->id, q_target->tag, "No action (control = 1)");
    }

    return 0;
}

#else // __KERNEL__

#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/syscalls.h>
#include <linux/module.h>

int nymya_3310_anticontrol_not(struct nymya_qubit *q_ctrl, struct nymya_qubit *q_target) {
    int64_t ctrl_re, ctrl_im;

    if (!q_ctrl || !q_target) {
        pr_err("NYMYA: nymya_3310_anticontrol_not received NULL qubit pointer(s)\n");
        return -EINVAL;
    }

    ctrl_re = q_ctrl->amplitude.re;
    ctrl_im = q_ctrl->amplitude.im;

    __uint128_t mag_sq = (__uint128_t)ctrl_re * ctrl_re + (__uint128_t)ctrl_im * ctrl_im;

    const __uint128_t threshold_sq = (__uint128_t)(FIXED_POINT_SCALE / 2) * (FIXED_POINT_SCALE / 2);

    if (mag_sq < threshold_sq) {
        q_target->amplitude.re = -q_target->amplitude.re;
        q_target->amplitude.im = -q_target->amplitude.im;
        log_symbolic_event("ACNOT", q_target->id, q_target->tag, "Phase flipped due to control");
    } else {
        log_symbolic_event("ACNOT", q_target->id, q_target->tag, "No action (control = 1)");
    }

    return 0;
}

EXPORT_SYMBOL_GPL(nymya_3310_anticontrol_not);

SYSCALL_DEFINE2(nymya_3310_anticontrol_not,
    struct nymya_qubit __user *, user_ctrl,
    struct nymya_qubit __user *, user_target) {

    struct nymya_qubit k_ctrl, k_target;
    int ret;

    if (!user_ctrl || !user_target)
        return -EINVAL;

    if (copy_from_user(&k_ctrl, user_ctrl, sizeof(k_ctrl)))
        return -EFAULT;
    if (copy_from_user(&k_target, user_target, sizeof(k_target)))
        return -EFAULT;

    ret = nymya_3310_anticontrol_not(&k_ctrl, &k_target);
    if (ret)
        return ret;

    if (copy_to_user(user_target, &k_target, sizeof(k_target)))
        return -EFAULT;

    return 0;
}

#endif

