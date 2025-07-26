// src/nymya_3311_controlled_z.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_nymya_3311_controlled_z NYMYA_CZ_CODE

    #include <stdio.h>
    #include <stdlib.h>
    #include <complex.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/module.h> // ADDED: Required for EXPORT_SYMBOL_GPL
#endif

/**
 * nymya_3311_controlled_z - Apply a Controlled-Z gate to two qubits.
 * @q_ctrl: Pointer to the control qubit.
 * @q_target: Pointer to the target qubit.
 *
 * In user space, if the magnitude of the control qubit's amplitude
 * exceeds 0.5, this function applies a Z (phase flip) gate to the target.
 *
 * Returns 0 on success, or -1 on invalid input (null pointers).
 */
#ifndef __KERNEL__

int nymya_3311_controlled_z(nymya_qubit* q_ctrl, nymya_qubit* q_target) {
    if (!q_ctrl || !q_target) return -1;

    double ctrl_mag = cabs(q_ctrl->amplitude);

    if (ctrl_mag > 0.5) {
        q_target->amplitude *= -1;
        log_symbolic_event("CZ", q_target->id, q_target->tag, "Z applied via control");
    } else {
        log_symbolic_event("CZ", q_target->id, q_target->tag, "No phase shift (control = 0)");
    }

    return 0;
}

#else // __KERNEL__

/**
 * nymya_3311_controlled_z - Core kernel function for Controlled-Z gate.
 * @k_ctrl: Pointer to the kernel-space control qubit structure.
 * @k_target: Pointer to the kernel-space target qubit structure.
 *
 * This function applies the Controlled-Z gate logic: if the magnitude of the
 * control qubit's amplitude (in fixed-point) exceeds 0.5, it negates the
 * amplitude of the target qubit.
 *
 * Returns 0 on success, -EINVAL on null input.
 */
int nymya_3311_controlled_z(struct nymya_qubit *k_ctrl, struct nymya_qubit *k_target) {
    int64_t re, im;
    uint64_t re64, im64;
    uint64_t re_sq, im_sq;
    uint64_t mag_sq;

    if (!k_ctrl || !k_target) {
        pr_err("NYMYA: nymya_3311_controlled_z received NULL kernel qubit pointer(s)\n");
        return -EINVAL;
    }

    // Extract real and imaginary parts
    re = k_ctrl->amplitude.re;
    im = k_ctrl->amplitude.im;

    re64 = (uint64_t)(re < 0 ? -re : re);
    im64 = (uint64_t)(im < 0 ? -im : im);

    // Compute magnitude^2 using fixed-point math
    // The individual squared terms are scaled by FIXED_POINT_SCALE.
    // Their sum (mag_sq) is also scaled by FIXED_POINT_SCALE.
    re_sq = ((__uint128_t)re64 * re64) >> 32; // Result is (re^2 / 2^32) * 2^32 = re^2
    im_sq = ((__uint128_t)im64 * im64) >> 32; // Result is (im^2 / 2^32) * 2^32 = im^2
    mag_sq = re_sq + im_sq; // This sum is scaled by FIXED_POINT_SCALE

    // Fixed-point threshold for 0.5^2.
    // 0.5 in fixed-point is FIXED_POINT_SCALE / 2.
    // (0.5)^2 in fixed-point is (FIXED_POINT_SCALE / 2) * (FIXED_POINT_SCALE / 2) / FIXED_POINT_SCALE
    // Simplified: (FIXED_POINT_SCALE / 2) * (FIXED_POINT_SCALE / 2) >> 32
    // Or, more simply, 0.25 * FIXED_POINT_SCALE
    const uint64_t threshold_sq = (FIXED_POINT_SCALE / 4); // 0.25 * FIXED_POINT_SCALE

    if (mag_sq > threshold_sq) {
        k_target->amplitude.re = -k_target->amplitude.re;
        k_target->amplitude.im = -k_target->amplitude.im;
        log_symbolic_event("CZ", k_target->id, k_target->tag, "Z applied via control");
    } else {
        log_symbolic_event("CZ", k_target->id, k_target->tag, "No phase shift (control = 0)");
    }

    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3311_controlled_z);



// Export the symbol for this function so other kernel modules/code can call it directly.


/**
 * SYSCALL_DEFINE2(nymya_3311_controlled_z) - Kernel implementation of Controlled-Z gate.
 * @user_ctrl: User pointer to the control qubit structure.
 * @user_target: User pointer to the target qubit structure.
 *
 * This system call version copies the qubit structures from user space,
 * calculates the fixed-point magnitude of the control qubit, and applies
 * a Z gate (negating the amplitude) to the target qubit if the control
 * magnitude exceeds 0.5 (in fixed-point representation).
 *
 * Returns 0 on success, -EINVAL on null input, or -EFAULT on copy errors.
 */
SYSCALL_DEFINE2(nymya_3311_controlled_z,
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

    // Call the core logic function
    ret = nymya_3311_controlled_z(&k_ctrl, &k_target);

    if (ret) // Propagate error from core function
        return ret;

    if (copy_to_user(user_target, &k_target, sizeof(k_target)))
        return -EFAULT;

    return 0;
}

#endif

