// src/nymya_3330_rotate.c

#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/slab.h> // Required for kmalloc_array if used in other syscalls
    // No math.h in kernel; fixed-point math assumed for angle
#endif

#ifndef __KERNEL__

/**
 * nymya_3330_rotate - Applies a rotation gate to a single qubit around a specified axis (userland).
 * @q: Pointer to the qubit to rotate.
 * @axis: The axis of rotation ('X', 'Y', or 'Z', case-insensitive).
 * @theta: The angle of rotation in radians.
 *
 * This function acts as a wrapper, calling the appropriate single-qubit rotation
 * gate (Rx, Ry, or Rz) based on the specified axis.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL or an unknown axis is specified.
 */
int nymya_3330_rotate(nymya_qubit* q, char axis, double theta) {
    if (!q) return -1;

    switch (axis) {
        case 'X':
        case 'x':
            nymya_3319_rotate_x(q, theta);
            break;
        case 'Y':
        case 'y':
            nymya_3320_rotate_y(q, theta);
            break;
        case 'Z':
        case 'z':
            nymya_3321_rotate_z(q, theta);
            break;
        default:
            log_symbolic_event("ROTATE", q->id, q->tag, "Unknown axis");
            return -1;
    }

    log_symbolic_event("ROTATE", q->id, q->tag, "Axis rotation applied");
    return 0;
}

#else // __KERNEL__

/**
 * __do_sys_nymya_3330_rotate - Kernel system call implementation for single-qubit rotation.
 * @user_q: Pointer to the qubit structure in user space.
 * @axis: The axis of rotation ('X', 'Y', or 'Z', case-insensitive).
 * @theta_fp: The angle of rotation in fixed-point format.
 *
 * This function copies qubit data from user space, dispatches to the appropriate
 * fixed-point rotation function based on the axis, and then copies the modified
 * data back to user space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if the user qubit pointer is invalid or an unknown axis is specified.
 * - -EFAULT if copying data to/from user space fails.
 */
SYSCALL_DEFINE3(nymya_3330_rotate,
    struct nymya_qubit __user *, user_q,
    char, axis,
    int64_t, theta_fp) { // Changed theta to fixed-point (int64_t)

    struct nymya_qubit k_q; // Kernel-space copy of the qubit
    int ret = 0; // Return value for the rotation function calls

    // 1. Validate user pointer
    if (!user_q) {
        pr_err("nymya_3330_rotate: Invalid user qubit pointer\n");
        return -EINVAL;
    }

    // 2. Copy qubit data from user space to kernel space
    if (copy_from_user(&k_q, user_q, sizeof(k_q))) {
        pr_err("nymya_3330_rotate: Failed to copy k_q from user space\n");
        return -EFAULT;
    }

    // 3. Dispatch to the appropriate fixed-point rotation function
    switch (axis) {
        case 'X':
        case 'x':
            // Call the kernel version of rotate_x with fixed-point theta
            ret = nymya_3319_rotate_x(&k_q, theta_fp);
            break;
        case 'Y':
        case 'y':
            // Call the kernel version of rotate_y with fixed-point theta
            ret = nymya_3320_rotate_y(&k_q, theta_fp);
            break;
        case 'Z':
        case 'z':
            // Call the kernel version of rotate_z with fixed-point theta
            ret = nymya_3321_rotate_z(&k_q, theta_fp);
            break;
        default:
            // Log error for unknown axis and return invalid argument error
            log_symbolic_event("ROTATE", k_q.id, k_q.tag, "Unknown axis");
            return -EINVAL;
    }

    // 4. Log success if rotation was applied
    if (ret == 0) {
        log_symbolic_event("ROTATE", k_q.id, k_q.tag, "Axis rotation applied");
    } else {
        // If an error occurred in the underlying rotation function, log it
        pr_err("nymya_3330_rotate: Underlying rotation function failed with error %d\n", ret);
    }


    // 5. Copy modified qubit data back to user space
    if (copy_to_user(user_q, &k_q, sizeof(k_q))) {
        pr_err("nymya_3330_rotate: Failed to copy k_q to user space\n");
        return -EFAULT;
    }

    return ret; // Return the result of the rotation function
}

#endif

