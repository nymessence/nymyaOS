// src/nymya_3344_peres.c
//
// This file implements the nymya_3344_peres syscall, which applies a
// three-qubit Peres gate. The Peres gate is a reversible logic gate that
// combines a CNOT gate and a Toffoli (Controlled-Controlled-NOT) gate.
// It is often used as a building block for other quantum circuits due to
// its universality and reversibility.
//
// The implementation includes both userland and kernel-space versions,
// ensuring proper header inclusion and fixed-point arithmetic for kernel operations.

#include "nymya.h" // Common definitions like nymya_qubit, and gate macros

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>    // Userland: For standard I/O (e.g., in log_symbolic_event)
#define __NR_nymya_3344_peres NYMYA_PERES_CODE

    #include <stdlib.h>  // Userland: For general utilities
    #include <math.h>    // Userland: For complex math functions (if needed by sub-gates)
    #include <complex.h> // Userland: For _Complex double type (if needed by sub-gates)
#else
    #include <linux/kernel.h>   // Kernel: For pr_err and general kernel functions
    #include <linux/syscalls.h> // Kernel: For SYSCALL_DEFINE macros
    #include <linux/uaccess.h>  // Kernel: For copy_from_user, copy_to_user
    #include <linux/errno.h>    // Kernel: For error codes like -EINVAL, -EFAULT
    #include <linux/printk.h>   // Kernel: For pr_err
    #include <linux/module.h>   // Kernel: For EXPORT_SYMBOL_GPL
    // No math.h or complex.h in kernel; fixed-point math assumed for amplitude operations
#endif

#ifndef __KERNEL__

/**
 * nymya_3344_peres - Applies the Peres gate to three qubits (userland).
 * @q1: Pointer to the first qubit (often a control for the CNOT).
 * @q2: Pointer to the second qubit (often a control for the Margolis/Toffoli part).
 * @q3: Pointer to the third qubit (often the target for both CNOT and Margolis/Toffoli).
 *
 * This function implements the Peres gate using a decomposition into a CNOT
 * gate followed by a Margolis (Controlled-Controlled-NOT/Toffoli-like) gate.
 * Specifically, it applies CNOT(q1, q3) and then Margolis(q1, q2, q3).
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL (invalid input).
 */
int nymya_3344_peres(nymya_qubit* q1, nymya_qubit* q2, nymya_qubit* q3) {
    // Basic null pointer checks
    if (!q1 || !q2 || !q3) return -1;

    // Apply the sequence of gates that compose the Peres gate
    cnot(q1, q3);               // Apply CNOT with q1 as control, q3 as target
    nymya_3343_margolis(q1, q2, q3); // Apply Margolis with q1, q2 as controls, q3 as target

    // Log the symbolic event for traceability
    log_symbolic_event("PERES", q1->id, q1->tag, "Peres gate applied");
    return 0;
}

#else // __KERNEL__

/**
 * nymya_3344_peres_kernel_logic - Applies the Peres gate to three qubits (kernel-space core logic).
 * @q1: Pointer to the first qubit (often a control for the CNOT).
 * @q2: Pointer to the second qubit (often a control for the Margolis/Toffoli part).
 * @q3: Pointer to the third qubit (often the target for both CNOT and Margolis/Toffoli).
 *
 * This function implements the Peres gate using a decomposition into kernel-level
 * CNOT and Margolis (Controlled-Controlled-NOT/Toffoli-like) gates.
 * Specifically, it applies CNOT(q1, q3) and then Margolis(q1, q2, q3).
 *
 * This function operates on kernel-space qubit structures and is intended to be
 * called by the `nymya_3344_peres` syscall implementation after copying data
 * from user space.
 *
 * Returns:
 * - 0 on success.
 * - Error code from underlying gate operations (e.g., nymya_3309_controlled_not).
 */
int nymya_3344_peres_kernel_logic(struct nymya_qubit *q1,
                                  struct nymya_qubit *q2,
                                  struct nymya_qubit *q3)
{
    int ret = 0;

    // Apply the Peres gate logic for kernel space
    ret = nymya_3309_controlled_not(q1, q3); // CNOT with q1 as control, q3 as target
    if (ret) {
        pr_err("nymya_3344_peres_kernel_logic: CNOT failed, error %d\n", ret);
        return ret;
    }

    ret = nymya_3343_margolis(q1, q2, q3); // Margolis with q1, q2 as controls, q3 as target
    if (ret) {
        pr_err("nymya_3344_peres_kernel_logic: Margolis failed, error %d\n", ret);
        return ret;
    }

    // Log the symbolic event for traceability
    log_symbolic_event("PERES", q1->id, q1->tag, "Peres gate applied");

    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3344_peres_kernel_logic);


/**
 * SYSCALL_DEFINE3(nymya_3344_peres) - Kernel syscall for Peres gate.
 * @user_q1: User-space pointer to the first qubit structure.
 * @user_q2: User-space pointer to the second qubit structure.
 * @user_q3: User-space pointer to the third qubit structure.
 *
 * This syscall copies qubit data from user space to kernel space, applies the
 * Peres gate logic using kernel-space functions, and then copies the modified
 * data back to user space. The Peres gate is decomposed into kernel-level
 * CNOT and Margolis gates.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user qubit pointer is NULL.
 * - -EFAULT if copying data to/from user space fails.
 * - Error code from underlying gate operations (e.g., nymya_3309_controlled_not).
 */
SYSCALL_DEFINE3(nymya_3344_peres,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    struct nymya_qubit __user *, user_q3) {

    struct nymya_qubit k_q1, k_q2, k_q3; // Kernel-space copies of qubits
    int ret = 0; // Return value for syscall and gate operations

    // 1. Check for null pointers from user-space
    if (!user_q1 || !user_q2 || !user_q3) {
        pr_err("nymya_3344_peres: Null user qubit pointer(s)\n");
        return -EINVAL; // Invalid argument
    }

    // 2. Copy the qubit structures from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3344_peres: Failed to copy k_q1 from user\n");
        return -EFAULT; // Bad address
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3344_peres: Failed to copy k_q2 from user\n");
        return -EFAULT; // Bad address
    }
    if (copy_from_user(&k_q3, user_q3, sizeof(k_q3))) {
        pr_err("nymya_3344_peres: Failed to copy k_q3 from user\n");
        return -EFAULT; // Bad address
    }

    // 3. Apply the Peres gate logic for kernel space using the new helper function
    ret = nymya_3344_peres_kernel_logic(&k_q1, &k_q2, &k_q3);
    if (ret) {
        pr_err("nymya_3344_peres: Core logic failed, error %d\n", ret);
        return ret;
    }

    // 5. Copy the modified qubits back to user space
    // Note: All three qubits can be modified by the composite gate, so all should be copied back.
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3344_peres: Failed to copy k_q1 to user\n");
        ret = -EFAULT; // Bad address
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3344_peres: Failed to copy k_q2 to user\n");
        ret = -EFAULT; // Bad address
    }
    if (copy_to_user(user_q3, &k_q3, sizeof(k_q3))) {
        pr_err("nymya_3344_peres: Failed to copy k_q3 to user\n");
        ret = -EFAULT; // Bad address
    }

    return ret; // Return 0 on success, or error code if any operation failed
}

#endif
