// src/nymya_3329_fredkin.c

#include "nymya.h" // Includes complex_double and FIXED_POINT_SCALE

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <complex.h> // For _Complex double and cabs in userland
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/slab.h> // Required for kmalloc_array if used in other syscalls
    // No math.h or complex.h in kernel
#endif

#ifndef __KERNEL__

/**
 * nymya_3329_fredkin - Applies a Fredkin (Controlled-SWAP) gate to three qubits (userland).
 * @q_ctrl: Pointer to the control qubit.
 * @q1: Pointer to the first target qubit.
 * @q2: Pointer to the second target qubit.
 *
 * This function implements a Fredkin gate. If the control qubit's amplitude
 * magnitude squared is above a threshold (representing a '1' state), it swaps
 * the amplitudes of the two target qubits. Otherwise, no action is taken.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3329_fredkin(nymya_qubit* q_ctrl, nymya_qubit* q1, nymya_qubit* q2) {
    if (!q_ctrl || !q1 || !q2) return -1;

    // In userland, q_ctrl->amplitude is _Complex double.
    // Calculate the magnitude squared directly using cabs().
    double ctrl_mag_sq = cabs(q_ctrl->amplitude) * cabs(q_ctrl->amplitude);

    // Define the threshold for control qubit being '1' (0.5^2 = 0.25)
    const double threshold_sq = 0.25;

    // If the control qubit's amplitude magnitude squared is above the threshold, perform SWAP
    if (ctrl_mag_sq > threshold_sq) {
        _Complex double temp = q1->amplitude; // Use _Complex double for temp
        q1->amplitude = q2->amplitude;
        q2->amplitude = temp;

        log_symbolic_event("FREDKIN", q1->id, q1->tag, "Control triggered SWAP");
    } else {
        log_symbolic_event("FREDKIN", q1->id, q1->tag, "Control = 0, no action");
    }

    return 0;
}

#else // __KERNEL__

/**
 * __do_sys_nymya_3329_fredkin - Kernel system call implementation for Fredkin gate.
 * @user_q_ctrl: Pointer to the control qubit structure in user space.
 * @user_q1: Pointer to the first target qubit structure in user space.
 * @user_q2: Pointer to the second target qubit structure in user space.
 *
 * This function copies qubit data from user space, applies the Fredkin gate
 * using fixed-point complex arithmetic, and then copies the modified data back.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user pointer is invalid.
 * - -EFAULT if copying data to/from user space fails.
 */
SYSCALL_DEFINE3(nymya_3329_fredkin,
    struct nymya_qubit __user *, user_q_ctrl,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q_ctrl, k_q1, k_q2; // Kernel-space copies of qubits

    // 1. Validate user pointers
    if (!user_q_ctrl || !user_q1 || !user_q2) {
        pr_err("nymya_3329_fredkin: Invalid user qubit pointers\n");
        return -EINVAL;
    }

    // 2. Copy qubit data from user space to kernel space
    if (copy_from_user(&k_q_ctrl, user_q_ctrl, sizeof(k_q_ctrl))) {
        pr_err("nymya_3329_fredkin: Failed to copy k_q_ctrl from user space\n");
        return -EFAULT;
    }
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3329_fredkin: Failed to copy k_q1 from user space\n");
        return -EFAULT;
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3329_fredkin: Failed to copy k_q2 from user space\n");
        return -EFAULT;
    }

    // Calculate the squared magnitude of the control qubit's amplitude in fixed-point
    int64_t ctrl_mag_sq_fp = k_q_ctrl.amplitude.re * k_q_ctrl.amplitude.re +
                             k_q_ctrl.amplitude.im * k_q_ctrl.amplitude.im;

    // Define the threshold for control qubit being '1' (0.5^2 = 0.25 in floating point)
    // Convert 0.25 to fixed-point squared value
    const int64_t threshold_sq_fp = (int64_t)(0.25 * FIXED_POINT_SCALE * FIXED_POINT_SCALE);

    // If the control qubit's amplitude magnitude squared is above the threshold, perform SWAP
    if (ctrl_mag_sq_fp > threshold_sq_fp) {
        complex_double temp = k_q1.amplitude; // Use complex_double for temp
        k_q1.amplitude = k_q2.amplitude;
        k_q2.amplitude = temp;

        log_symbolic_event("FREDKIN", k_q1.id, k_q1.tag, "Control triggered SWAP");
    } else {
        log_symbolic_event("FREDKIN", k_q1.id, k_q1.tag, "Control = 0, no action");
    }

    // 3. Copy modified qubit data back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3329_fredkin: Failed to copy k_q1 to user space\n");
        return -EFAULT;
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3329_fredkin: Failed to copy k_q2 to user space\n");
        return -EFAULT;
    }
    // No need to copy k_q_ctrl back if it's only read and not modified.
    // If it *could* be modified by the gate, it would need to be copied back.
    // For Fredkin, control qubit is typically unchanged.

    return 0; // Success
}

#endif

