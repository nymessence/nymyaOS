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
    #include <linux/module.h> // Required for EXPORT_SYMBOL_GPL

    /**
     * @brief   Applies the Anti-Controlled-NOT (ACNOT) gate logic on kernel-space qubit data.
     * @details If the control qubit's amplitude magnitude squared is less than 0.5^2 (pre-calculated
     *          as 'threshold' using FIXED_POINT_SCALE), the target qubit's amplitude sign
     *          (both real and imaginary parts) is flipped.
     *          Uses fixed-point arithmetic for amplitude calculations.
     *
     * @param   k_ctrl      Pointer to the kernel-space control qubit data. This data is read-only.
     * @param   k_target    Pointer to the kernel-space target qubit data. This data is modified
     *                      in place if the anti-control condition is met.
     * @return  0 on success. (Currently, this function does not return specific error codes
     *          as its input is assumed to be valid kernel data).
     */
    int nymya_3310_anticontrol_not_core(const struct nymya_qubit *k_ctrl, struct nymya_qubit *k_target) {
        int64_t ctrl_re, ctrl_im;
        __uint128_t mag_sq;
        // Pre-calculate the threshold for fixed-point magnitude squared comparison (0.5^2)
        const __uint128_t threshold = (__uint128_t)(0.5 * FIXED_POINT_SCALE) * (0.5 * FIXED_POINT_SCALE);

        ctrl_re = k_ctrl->amplitude.re;
        ctrl_im = k_ctrl->amplitude.im;

        // Compute magnitude squared in fixed-point
        mag_sq = (__uint128_t)ctrl_re * ctrl_re + (__uint128_t)ctrl_im * ctrl_im;

        if (mag_sq < threshold) {
            // Flip target amplitude sign (multiply by -1)
            k_target->amplitude.re = -k_target->amplitude.re;
            k_target->amplitude.im = -k_target->amplitude.im;

            log_symbolic_event("ACNOT", k_target->id, k_target->tag, "NOT via anti-control (ctrl=0)");
        } else {
            log_symbolic_event("ACNOT", k_target->id, k_target->tag, "No action (control = 1)");
        }

        return 0;
    }


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
    int ret; // Variable to hold return value from core logic

    if (!user_ctrl || !user_target)
        return -EINVAL;

    if (copy_from_user(&k_ctrl, user_ctrl, sizeof(k_ctrl)))
        return -EFAULT;
    if (copy_from_user(&k_target, user_target, sizeof(k_target)))
        return -EFAULT;

    // Call the extracted core logic function
    ret = nymya_3310_anticontrol_not_core(&k_ctrl, &k_target);
    // If the core function returned an error, propagate it.
    // Currently, nymya_3310_anticontrol_not_core always returns 0.

    if (copy_to_user(user_target, &k_target, sizeof(k_target)))
        return -EFAULT;

    return ret;
}

#endif