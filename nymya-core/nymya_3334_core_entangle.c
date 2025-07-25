// src/nymya_3334_core_entangle.c

#include "nymya.h" // Common definitions like complex_double

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>    // Userland only
#define __NR_nymya_3334_core_entangle NYMYA_CORE_ENTANGLE_CODE

    #include <stdlib.h>  // Userland only
    #include <math.h>    // For complex math functions in userland
    #include <complex.h> // For _Complex double in userland
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h> // For -EINVAL, -EFAULT
    // No math.h or complex.h in kernel; fixed-point math assumed for amplitude operations
#endif

#ifndef __KERNEL__

// Userland implementation
/**
 * nymya_3334_core_entangle - Applies a "core entangle" gate to two qubits (userland).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function applies a specific two-qubit entangling operation,
 * typically a Bell state preparation (Hadamard on q1, then CNOT with q1 as control, q2 as target).
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3334_core_entangle(nymya_qubit* q1, nymya_qubit* q2) {
    if (!q1 || !q2) return -1;

    // Bell state preparation: Hadamard on q1, then CNOT with q1 as control, q2 as target
    nymya_3308_hadamard_gate(q1);
    nymya_3309_controlled_not(q1, q2);

    log_symbolic_event("CORE_EN", q1->id, q1->tag, "Core entanglement applied");
    return 0;
}

#else // __KERNEL__

/**
 * SYSCALL_DEFINE2(nymya_3334_core_entangle) - Kernel syscall for "core entangle" gate.
 * @user_q1: User-space pointer to the first qubit.
 * @user_q2: User-space pointer to the second qubit.
 *
 * This syscall copies qubit data from user space, applies the "core entangle" gate
 * logic (Bell state preparation) using kernel-space functions, and then copies the
 * modified data back.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user pointer is invalid.
 * - -EFAULT if copying data to/from user space fails.
 * - Error code from underlying gate operations (e.g., nymya_3308_hadamard_gate, nymya_3309_controlled_not).
 */
SYSCALL_DEFINE2(nymya_3334_core_entangle,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2; // Kernel-space copies
    int ret = 0;

    // 1. Validate user pointers
    if (!user_q1 || !user_q2) {
        pr_err("nymya_3334_core_entangle: Invalid user qubit pointers\n");
        return -EINVAL;
    }

    // 2. Copy qubit data from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3334_core_entangle: Failed to copy k_q1 from user space\n");
        return -EFAULT;
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3334_core_entangle: Failed to copy k_q2 from user space\n");
        return -EFAULT;
    }

    // 3. Apply the "core entangle" logic for kernel space (Bell state preparation)
    // Call the kernel versions of Hadamard and CNOT gates
    ret = nymya_3308_hadamard_gate(&k_q1);
    if (ret) {
        pr_err("nymya_3334_core_entangle: Hadamard gate failed on q1, error %d\n", ret);
        return ret;
    }

    ret = nymya_3309_controlled_not(&k_q1, &k_q2);
    if (ret) {
        pr_err("nymya_3334_core_entangle: CNOT gate failed, error %d\n", ret);
        return ret;
    }

    log_symbolic_event("CORE_EN", k_q1.id, k_q1.tag, "Core entanglement applied");

    // 4. Copy modified qubit data back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3334_core_entangle: Failed to copy k_q1 to user space\n");
        return -EFAULT;
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3334_core_entangle: Failed to copy k_q2 to user space\n");
        return -EFAULT;
    }

    return 0; // Return 0 for success
}

#endif

