// src/nymya_3332_berkeley.c

#include "nymya.h" // This should bring in common definitions and conditional includes

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>    // Userland only
#define __NR_nymya_3332_berkeley NYMYA_BERKELEY_CODE

    #include <stdlib.h>  // Userland only
    #include <math.h>    // Userland only
    #include <complex.h> // Userland only
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/types.h> // For __int128
    #include <linux/slab.h> // For kmalloc_array if needed
    #include <linux/module.h> // Ensure this is present
    // No math.h or complex.h in kernel
#endif

// Fixed-point constants and helper functions for kernel (if not already in nymya.h)
#ifdef __KERNEL__

// Define PI and PI/2 in fixed-point for kernel calculations if not already in nymya.h
// These are now defined in nymya.h and should not be redefined here.
// #ifndef FIXED_POINT_PI
// #define FIXED_POINT_PI (int64_t)(3.141592653589793 * FIXED_POINT_SCALE)
// #endif
// #ifndef FIXED_POINT_PI_DIV_2
// #define FIXED_POINT_PI_DIV_2 (int64_t)(1.5707963267948966 * FIXED_POINT_SCALE)
// #endif

// fixed_point_mul, fixed_point_cos, fixed_point_sin are now defined as static inline in nymya.h
// and should NOT be redefined here.

#endif // __KERNEL__

#ifndef __KERNEL__

/**
 * nymya_3332_berkeley - Applies a Berkeley gate to two qubits (userland version).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @theta: The gate parameter.
 *
 * This function implements a two-qubit Berkeley gate, which is a parameterized
 * entangling gate. The exact transformation depends on the parameter $\theta$.
 * This is a composite gate typically implemented as CNOT-Phase-CNOT.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3332_berkeley(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    if (!q1 || !q2) return -1;

    // Assuming userland versions of these gates exist and accept double theta
    nymya_3309_controlled_not(q1, q2);
    nymya_3316_phase_gate(q2, theta); // This expects double theta
    nymya_3309_controlled_not(q1, q2);

    log_symbolic_event("BERKELEY", q1->id, q1->tag, "Berkeley entangler applied");
    return 0;
}

#else // __KERNEL__

/**
 * nymya_3332_berkeley - Core kernel-space logic for the Berkeley gate.
 * @q1: Pointer to the first qubit (kernel space).
 * @q2: Pointer to the second qubit (kernel space).
 * @theta_fp: The gate parameter in fixed-point format.
 *
 * This function implements the Berkeley gate logic by calling kernel-space
 * versions of CNOT and Phase gates. It also logs the gate application.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any qubit pointer is NULL, or if a sub-gate fails.
 */
int nymya_3332_berkeley(struct nymya_qubit* q1, struct nymya_qubit* q2, int64_t theta_fp) {
    int ret = 0;
    if (!q1 || !q2) {
        pr_err("nymya_3332_berkeley: Null qubit pointer in core logic.\n");
        return -EINVAL; // Use kernel-style error codes
    }

    // Call kernel-space versions of the sub-gates
    // These functions must exist and be callable from kernel context.
    // Their prototypes should be in nymya.h (for kernel).
    ret = nymya_3309_controlled_not(q1, q2);
    if (ret) {
        pr_err("nymya_3332_berkeley: CNOT failed with error %d\n", ret);
        return ret;
    }
    ret = nymya_3316_phase_gate(q2, theta_fp); // This now expects int64_t theta_fp
    if (ret) {
        pr_err("nymya_3332_berkeley: Phase gate failed with error %d\n", ret);
        return ret;
    }
    ret = nymya_3309_controlled_not(q1, q2);
    if (ret) {
        pr_err("nymya_3332_berkeley: Second CNOT failed with error %d\n", ret);
        return ret;
    }

    // Log event after successful application
    log_symbolic_event("BERKELEY", q1->id, q1->tag, "Berkeley entangler applied");

    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3332_berkeley);



/**
 * SYSCALL_DEFINE3(nymya_3332_berkeley) - Kernel syscall for Berkeley gate.
 * @user_q1: User-space pointer to the first qubit.
 * @user_q2: User-space pointer to the second qubit.
 * @theta_fp: The gate parameter in fixed-point (int64_t) format.
 *
 * This syscall copies qubit data from user space, applies the Berkeley gate
 * using the core kernel-space helper function, and then copies the modified data back.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user pointer is invalid.
 * - -EFAULT if copying data to/from user space fails.
 * - Error code from underlying gate operations.
 */
SYSCALL_DEFINE3(nymya_3332_berkeley,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    int64_t, theta_fp) {

    struct nymya_qubit k_q1, k_q2;
    int ret = 0;

    // 1. Validate user pointers
    if (!user_q1 || !user_q2) {
        pr_err("nymya_3332_berkeley_syscall: Invalid user qubit pointers\n");
        return -EINVAL;
    }

    // 2. Copy qubit data from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3332_berkeley_syscall: Failed to copy k_q1 from user space\n");
        return -EFAULT;
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3332_berkeley_syscall: Failed to copy k_q2 from user space\n");
        return -EFAULT;
    }

    // 3. Call the refactored kernel-space core function
    ret = nymya_3332_berkeley(&k_q1, &k_q2, theta_fp);

    // 4. Copy data back to user space if successful
    if (ret == 0) {
        if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
            return -EFAULT;

        if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
            return -EFAULT;
    } else {
        pr_err("nymya_3332_berkeley_syscall: Kernel Berkeley operation failed with error %d\n", ret);
    }

    return ret;
}
#endif
