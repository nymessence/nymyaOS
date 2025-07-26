// src/nymya_3351_tessellated_hex_rhombi.c
//
// This file implements the nymya_3351_tessellated_hex_rhombi syscall, which
// applies operations to a set of qubits arranged in a tessellated pattern of
// hexagonal-rhombic shapes. This function processes qubits in groups of seven
// (one central, six surrounding), applying specific entangling sequences
// (Hadamard and CNOTs) to each hex-rhombi unit.
//
// The implementation includes both userland and kernel-space versions,
// ensuring proper header inclusion, memory management, and fixed-point
// arithmetic for kernel operations.

#include "nymya.h" // Common definitions like nymya_qubit, and gate macros

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>    // Userland: For standard I/O (e.g., in log_symbolic_event)
#define __NR_nymya_3351_tessellated_hex_rhombi NYMYA_TESS_HEX_RHOMBI_CODE

#include <stdlib.h>  // Userland: For general utilities (e.g., size_t)
#include <math.h>    // Userland: For complex math functions (if needed by sub-gates)
#include <complex.h> // Userland: For _Complex double type (if needed by sub-gates)
#else
#include <linux/kernel.h>   // Kernel: For pr_err and general kernel functions
#include <linux/syscalls.h> // Kernel: For SYSCALL_DEFINE macros
#include <linux/uaccess.h>  // Kernel: For copy_from_user, copy_to_user
#include <linux/errno.h>    // Kernel: For error codes like -EINVAL, -EFAULT, -ENOMEM
#include <linux/printk.h>   // Kernel: For pr_err
#include <linux/slab.h>     // Kernel: For kmalloc, kfree, kmalloc_array
#include <linux/module.h>   // Kernel: For EXPORT_SYMBOL_GPL
#endif

#ifndef __KERNEL__

/**
 * nymya_3351_tessellated_hex_rhombi - Applies operations across a tessellated pattern of hexagonal-rhombic units (userland).
 * @q: An array of pointers to nymya_qubit objects.
 * @count: The total number of qubits in the array.
 *
 * This function processes the provided qubits in groups of seven, forming
 * "hex-rhombi" units (one central qubit and six surrounding ones). For each
 * complete unit, it applies a sequence of gates:
 * 1. Hadamard on each of the six outer qubits.
 * 2. CNOTs between the central qubit (q[base]) and each of the six outer qubits.
 * 3. CNOTs to form rhombi edges: between adjacent outer qubits and connecting
 * them back to the central qubit in a cyclic manner.
 * The function only processes full hex-rhombi units, so any remaining qubits
 * (if count is not a multiple of 7) are ignored.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit array is NULL or if `count` is less than 7 (cannot form a unit).
 * - -1 if any individual qubit pointer within a processed unit is NULL.
 */
int nymya_3351_tessellated_hex_rhombi(nymya_qubit* q[], size_t count) {
    // Basic null pointer and minimum count check
    if (!q || count < 7) return -1;

    // Calculate the number of complete hex-rhombi groups that can be formed
    size_t groups = count / 7;

    // Iterate through each hex-rhombi group
    for (size_t g = 0; g < groups; g++) {
        size_t base = 7 * g; // Base index for the current group's qubits

        // Check for null pointer for the central qubit
        if (!q[base]) return -1;

        // Entangle center with all six outer qubits
        for (int i = 1; i <= 6; i++) {
            if (!q[base + i]) return -1; // Check for null pointer for outer qubits
            hadamard(q[base + i]);   // Hadamard on outer qubit
            cnot(q[base], q[base + i]); // CNOT with central as control, outer as target
        }

        // Build rhombi edges: CNOTs between adjacent outer qubits and back to center
        // Loop for (q[1]-q[2]-q[0]-q[6]), (q[2]-q[3]-q[0]-q[1]), ...
        for (int i = 1; i <= 5; i++) {
            cnot(q[base + i], q[base + i + 1]);   // CNOT between adjacent outer qubits
            cnot(q[base + i + 1], q[base]);       // CNOT with outer as control, central as target
        }
        // Complete the cycle for the last rhombus: (q[6], q[1], q[0])
        cnot(q[base + 6], q[base + 1]);   // CNOT between q[6] and q[1]
        cnot(q[base + 1], q[base]);       // CNOT with q[1] as control, central q[0] as target

        // Log the symbolic event for traceability, using the central qubit of the group
        log_symbolic_event("HEX_RHOM_T", q[base]->id, q[base]->tag, "Hex→3 rhombi tessellate");
    }
    return 0; // Success
}

#else // __KERNEL__

/**
 * nymya_3351_tessellated_hex_rhombi_core - Applies operations across a tessellated pattern of hexagonal-rhombic units (kernel-space).
 * @k_qubits: An array of pointers to kernel-space nymya_qubit structures.
 * @count: The total number of qubits in the array.
 *
 * This function processes the provided qubits in groups of seven, forming
 * "hex-rhombi" units (one central qubit and six surrounding ones). For each
 * complete unit, it applies a sequence of gates: Hadamard, CNOTs, and then
 * CNOTs to form rhombi edges. It operates directly on kernel-space qubit data.
 * If `count` is less than 7, no units can be formed, and the function returns 0.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any individual qubit pointer within a processed unit is NULL.
 * - Error code from underlying gate operations (e.g., nymya_3308_hadamard_gate,
 *   nymya_3309_controlled_not).
 */
int nymya_3351_tessellated_hex_rhombi_core(struct nymya_qubit **k_qubits, size_t count) {
    size_t groups;
    int ret = 0;

    // Calculate the number of complete hex-rhombi groups that can be formed
    if (count < 7) {
        // No full groups can be formed, so nothing to do. Return success.
        return 0;
    }
    groups = count / 7;

    // Apply the tessellated hex-rhombi logic for kernel space
    // Iterate through each hex-rhombi group
    for (size_t g = 0; g < groups; g++) {
        size_t base = 7 * g; // Base index for the current group's qubits

        // Check for null pointer for the central qubit in kernel space (should be handled by syscall, but defensive)
        if (!k_qubits[base]) {
            pr_err("nymya_3351_tessellated_hex_rhombi_core: Central qubit k_q[%zu] is NULL\n", base);
            return -EINVAL;
        }

        // Entangle center with all six outer qubits
        for (int j = 1; j <= 6; j++) {
            if (!k_qubits[base + j]) { // Defensive check
                pr_err("nymya_3351_tessellated_hex_rhombi_core: Outer qubit k_q[%zu+%d] is NULL\n", base, j);
                return -EINVAL;
            }
            ret = nymya_3308_hadamard_gate(k_qubits[base + j]); // Hadamard on outer qubit
            if (ret) {
                pr_err("nymya_3351_tessellated_hex_rhombi_core: Hadamard on outer qubit %d failed, error %d\n", j, ret);
                return ret;
            }
            ret = nymya_3309_controlled_not(k_qubits[base], k_qubits[base + j]); // CNOT(central, outer)
            if (ret) {
                pr_err("nymya_3351_tessellated_hex_rhombi_core: CNOT(central, outer %d) failed, error %d\n", j, ret);
                return ret;
            }
        }

        // Build rhombi edges: CNOTs between adjacent outer qubits and back to center
        for (int j = 1; j <= 5; j++) {
            if (!k_qubits[base + j] || !k_qubits[base + j + 1]) { // Defensive check
                pr_err("nymya_3351_tessellated_hex_rhombi_core: Rhombi qubits k_q[%zu+%d] or k_q[%zu+%d] are NULL\n", base, j, base, j + 1);
                return -EINVAL;
            }
            ret = nymya_3309_controlled_not(k_qubits[base + j], k_qubits[base + j + 1]); // CNOT between adjacent outer
            if (ret) {
                pr_err("nymya_3351_tessellated_hex_rhombi_core: CNOT(outer %d, outer %d) failed, error %d\n", j, j + 1, ret);
                return ret;
            }
            ret = nymya_3309_controlled_not(k_qubits[base + j + 1], k_qubits[base]); // CNOT(outer, central)
            if (ret) {
                pr_err("nymya_3351_tessellated_hex_rhombi_core: CNOT(outer %d, central) failed, error %d\n", j + 1, ret);
                return ret;
            }
        }
        // Complete the cycle for the last rhombus: (q[6], q[1], q[0])
        if (!k_qubits[base + 6] || !k_qubits[base + 1]) { // Defensive check
            pr_err("nymya_3351_tessellated_hex_rhombi_core: Last rhombus qubits k_q[%zu+6] or k_q[%zu+1] are NULL\n", base, base);
            return -EINVAL;
        }
        ret = nymya_3309_controlled_not(k_qubits[base + 6], k_qubits[base + 1]); // CNOT(q[6], q[1])
        if (ret) {
            pr_err("nymya_3351_tessellated_hex_rhombi_core: CNOT(q[6], q[1]) failed, error %d\n", ret);
            return ret;
        }
        ret = nymya_3309_controlled_not(k_qubits[base + 1], k_qubits[base]); // CNOT(q[1], q[0])
        if (ret) {
            pr_err("nymya_3351_tessellated_hex_rhombi_core: CNOT(q[1], q[0]) failed, error %d\n", ret);
            return ret;
        }

        // Log the symbolic event for traceability
        log_symbolic_event("HEX_RHOM_T", k_qubits[base]->id, k_qubits[base]->tag, "Hex→3 rhombi tessellate");
    }
    return 0; // Success
}
EXPORT_SYMBOL_GPL(nymya_3351_tessellated_hex_rhombi_core);


/**
 * SYSCALL_DEFINE2(nymya_3351_tessellated_hex_rhombi) - Kernel syscall for tessellated hex-rhombi lattice operation.
 * @user_q_array: User-space pointer to an array of user-space qubit pointers.
 * (i.e., `struct nymya_qubit __user *[]`)
 * @count: The total number of qubits in the array provided by user space.
 *
 * This syscall copies the array of user-space qubit pointers, then copies each
 * individual qubit structure from user space to kernel space. It applies the
 * tessellated hexagonal-rhombic lattice gate logic using the
 * `nymya_3351_tessellated_hex_rhombi_core` function to groups of seven qubits,
 * and finally copies the modified qubit data back to user space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if the user_q_array pointer is NULL or `count` is less than 7,
 * or if any individual user qubit pointer within a unit is NULL.
 * - -EFAULT if copying data to/from user space fails for any qubit or pointer.
 * - -ENOMEM if kernel memory allocation fails.
 * - Error code from underlying gate operations (e.g., nymya_3308_hadamard_gate)
 *   propagated from `nymya_3351_tessellated_hex_rhombi_core`.
 */
SYSCALL_DEFINE2(nymya_3351_tessellated_hex_rhombi,
    struct nymya_qubit __user * __user *, user_q_array,
    size_t, count) {

    struct nymya_qubit **k_qubits = NULL; // Array of pointers to kernel-space qubit data
    struct nymya_qubit __user **user_qubit_ptrs = NULL; // Array to hold user-space pointers
    int ret = 0; // Return value for syscall and gate operations
    size_t i; // Loop counter

    // 1. Basic validation: Check for null array pointer and minimum count
    if (!user_q_array || count < 7) {
        pr_err("nymya_3351_tessellated_hex_rhombi: Invalid user_q_array or count (%zu)\n", count);
        return -EINVAL;
    }

    // 2. Allocate kernel memory for the array of user-space qubit pointers
    user_qubit_ptrs = kmalloc_array(count, sizeof(struct nymya_qubit __user *), GFP_KERNEL);
    if (!user_qubit_ptrs) {
        pr_err("nymya_3351_tessellated_hex_rhombi: Failed to allocate memory for user_qubit_ptrs array\n");
        return -ENOMEM;
    }

    // Copy the array of user-space qubit pointers from user space
    if (copy_from_user(user_qubit_ptrs, user_q_array, count * sizeof(struct nymya_qubit __user *))) {
        pr_err("nymya_3351_tessellated_hex_rhombi: Failed to copy user qubit pointers array\n");
        ret = -EFAULT;
        goto cleanup_user_ptrs_array;
    }

    // 3. Allocate kernel memory for the array of kernel-space qubit structures
    k_qubits = kmalloc_array(count, sizeof(struct nymya_qubit *), GFP_KERNEL);
    if (!k_qubits) {
        pr_err("nymya_3351_tessellated_hex_rhombi: Failed to allocate memory for k_qubits array\n");
        ret = -ENOMEM;
        goto cleanup_user_ptrs_array;
    }

    // Initialize k_qubits pointers to NULL for safe cleanup
    for (i = 0; i < count; i++) {
        k_qubits[i] = NULL;
    }

    // 4. Allocate memory for each individual qubit and copy data from user space
    for (i = 0; i < count; i++) {
        // Check if individual user-space qubit pointer is NULL
        if (!user_qubit_ptrs[i]) {
            pr_err("nymya_3351_tessellated_hex_rhombi: Null individual user qubit pointer at index %zu\n", i);
            ret = -EINVAL;
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }

        // Allocate memory for the kernel-space copy of the qubit
        k_qubits[i] = kmalloc(sizeof(struct nymya_qubit), GFP_KERNEL);
        if (!k_qubits[i]) {
            pr_err("nymya_3351_tessellated_hex_rhombi: Failed to allocate memory for k_qubit[%zu]\n", i);
            ret = -ENOMEM; // Out of memory
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }

        // Copy the actual qubit data from user space into the allocated kernel memory
        if (copy_from_user(k_qubits[i], user_qubit_ptrs[i], sizeof(struct nymya_qubit))) {
            pr_err("nymya_3351_tessellated_hex_rhombi: Failed to copy k_qubit[%zu] data from user\n", i);
            ret = -EFAULT; // Bad address
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }
    }

    // 5. Call the core logic function with kernel-space qubits
    ret = nymya_3351_tessellated_hex_rhombi_core(k_qubits, count);
    if (ret) {
        // Error already logged by core function, just propagate it
        goto cleanup_k_qubits;
    }

    // 6. Copy the modified qubits back to user space
    // Only proceed if no errors occurred during gate applications (ret == 0 at this point)
    for (i = 0; i < count; i++) {
        // Copy the actual qubit data from kernel memory back to user space
        if (copy_to_user(user_qubit_ptrs[i], k_qubits[i], sizeof(struct nymya_qubit))) {
            pr_err("nymya_3351_tessellated_hex_rhombi: Failed to copy k_qubit[%zu] to user\n", i);
            // Set ret to -EFAULT if any copy fails, but continue to free memory
            // This ensures all allocated kernel memory is freed even if some copy_to_user fails.
            if (ret == 0) ret = -EFAULT;
        }
    }

cleanup_k_qubits:
    // 7. Free all allocated kernel memory for individual qubits
    for (size_t j = 0; j < count; j++) {
        if (k_qubits && k_qubits[j]) { // Check if k_qubits array and individual pointer are valid
            kfree(k_qubits[j]);
            k_qubits[j] = NULL; // Prevent double free
        }
    }
    if (k_qubits) {
        kfree(k_qubits); // Free the array of pointers itself
        k_qubits = NULL;
    }

cleanup_user_ptrs_array:
    if (user_qubit_ptrs) {
        kfree(user_qubit_ptrs); // Free the array holding user-space pointers
        user_qubit_ptrs = NULL;
    }

    return ret; // Return 0 on success, or error code if any operation failed
}

#endif
