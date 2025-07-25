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
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_nymya_3309_controlled_not NYMYA_CNOT_CODE

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <complex.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/module.h> // ADDED: Required for EXPORT_SYMBOL_GPL
#endif

#ifndef __KERNEL__

/*
 * Function: nymya_3309_controlled_not
 * -----------------------------------
 * Applies the Controlled-NOT gate.
 * If control qubit's amplitude magnitude > 0.5, flips the sign of the target's amplitude.
 *
 * Parameters:
 * q_ctrl   - pointer to control qubit
 * q_target - pointer to target qubit
 *
 * Returns:
 * 0 on success, -1 if either pointer is NULL
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

#else // __KERNEL__

/**
 * nymya_3309_controlled_not - Core kernel function for Controlled-NOT (CNOT) gate.
 * @q_ctrl: Pointer to the control qubit.
 * @q_target: Pointer to the target qubit.
 *
 * This function applies the CNOT gate logic: if the control qubit's amplitude
 * magnitude squared is above a threshold, it flips the sign of the target qubit's amplitude.
 * This function is designed to be called directly by other kernel code.
 *
 * Returns:
 * 0 on success.
 * -EINVAL if either qubit pointer is NULL.
 */
int nymya_3309_controlled_not(struct nymya_qubit *q_ctrl, struct nymya_qubit *q_target) {
    int64_t ctrl_re, ctrl_im;

    if (!q_ctrl || !q_target) {
        pr_err("NYMYA: nymya_3309_controlled_not received NULL qubit pointer(s)\n");
        return -EINVAL;
    }

    ctrl_re = q_ctrl->amplitude.re;
    ctrl_im = q_ctrl->amplitude.im;

    // Calculate magnitude squared in fixed-point: (re^2 + im^2)
    // The result is still in a higher fixed-point scale, so we compare with a scaled threshold.
    __uint128_t mag_sq = (__uint128_t)ctrl_re * ctrl_re + (__uint128_t)ctrl_im * ctrl_im;

    // Threshold magnitude ~0.5 in fixed point squared.
    // 0.5 * FIXED_POINT_SCALE is the fixed-point representation of 0.5.
    // (0.5 * FIXED_POINT_SCALE)^2 is the squared fixed-point threshold.
    // Note: FIXED_POINT_SCALE is typically 2^32, so 0.5 * FIXED_POINT_SCALE is 2^31.
    // (2^31)^2 = 2^62.
    // The mag_sq calculation results in a value scaled by (FIXED_POINT_SCALE)^2.
    // So, the threshold also needs to be scaled by (FIXED_POINT_SCALE)^2 for direct comparison.
    const __uint128_t threshold_sq = (__uint128_t)(FIXED_POINT_SCALE / 2) * (FIXED_POINT_SCALE / 2);

    if (mag_sq > threshold_sq) {
        // Flip target amplitude sign (multiply by -1)
        q_target->amplitude.re = -q_target->amplitude.re;
        q_target->amplitude.im = -q_target->amplitude.im;

        log_symbolic_event("CNOT", q_target->id, q_target->tag, "NOT applied via control");
    } else {
        log_symbolic_event("CNOT", q_target->id, q_target->tag, "No action (control = 0)");
    }

    return 0;
}
// Export the symbol for this function so other kernel modules/code can call it directly.
EXPORT_SYMBOL_GPL(nymya_3309_controlled_not);


/*
 * Syscall: nymya_3309_controlled_not
 * ----------------------------------
 * Kernel-space syscall handler for CNOT gate.
 * Checks control qubit magnitude; flips target amplitude sign if control is 'on'.
 *
 * Parameters:
 * user_ctrl   - pointer to user-space control qubit
 * user_target - pointer to user-space target qubit
 *
 * Returns:
 * 0 on success
 * -EINVAL if either pointer is NULL
 * -EFAULT if copying from/to user memory fails
 */
SYSCALL_DEFINE2(nymya_3309_controlled_not,
    struct nymya_qubit __user *, user_ctrl,
    struct nymya_qubit __user *, user_target) {

    struct nymya_qubit k_ctrl, k_target;
    int ret;

    if (!user_ctrl || !user_target)
        return -EINVAL;

    // Copy qubits from user space
    if (copy_from_user(&k_ctrl, user_ctrl, sizeof(k_ctrl)))
        return -EFAULT;
    if (copy_from_user(&k_target, user_target, sizeof(k_target)))
        return -EFAULT;

    // Call the core logic function
    ret = nymya_3309_controlled_not(&k_ctrl, &k_target);

    if (ret) // Propagate error from core function
        return ret;

    // Copy modified target qubit back to user space
    if (copy_to_user(user_target, &k_target, sizeof(k_target)))
        return -EFAULT;

    // Note: Control qubit is not modified, so no need to copy it back.
    return 0;
}

#endif

