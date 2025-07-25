// src/nymya_3353_flower_of_life.c
//
// This file implements the nymya_3353_flower_of_life syscall, which applies
// operations to a set of qubits arranged in a pattern inspired by the
// "Flower of Life" geometric design. This involves entangling a central
// qubit with surrounding ones and then creating connections between the
// outer qubits to form a complex entangled structure.
//
// The implementation includes both userland and kernel-space versions,
// ensuring proper header inclusion, robust memory management, and fixed-point
// arithmetic for kernel operations.

#include "nymya.h" // Common definitions like nymya_qubit, and gate macros

#ifndef __KERNEL__
    #include <stdio.h>    // Userland: For standard I/O (e.g., in log_symbolic_event)
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
#endif

#ifndef __KERNEL__

/**
 * nymya_3353_flower_of_life - Applies operations related to the "Flower of Life" pattern to qubits (userland).
 * @q: An array of pointers to nymya_qubit objects.
 * @count: The total number of qubits in the array.
 *
 * This function simulates entanglement patterns inspired by the "Flower of Life"
 * geometry. It expects at least 19 qubits to form a central qubit and two rings
 * of 6 and 12 qubits, respectively. The operations include:
 * 1. Applying Hadamard gates to all 19 qubits.
 * 2. Entangling the central qubit (q[0]) with all other 18 qubits using CNOTs.
 * 3. Applying CNOTs between adjacent qubits in the first ring (q[1] to q[6] cyclically).
 * 4. Applying CNOTs between adjacent qubits in the second ring (q[7] to q[18] cyclically).
 * The function only processes a full 19-qubit unit if `count` is sufficient.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit array is NULL or if `count` is less than 19.
 * - -1 if any individual qubit pointer within the processed unit is NULL.
 */
int nymya_3353_flower_of_life(nymya_qubit* q[], size_t count) {
    // Basic null pointer and minimum count check
    if (!q || count < 19) return -1;

    // Apply Hadamard to all 19 qubits
    for (size_t i = 0; i < 19; i++) {
        if (!q[i]) return -1; // Check for null pointer for each qubit
        hadamard(q[i]);
    }

    // Entangle the central qubit (q[0]) with all other 18 qubits
    for (size_t i = 1; i < 19; i++) {
        cnot(q[0], q[i]);
    }

    // Entangle qubits in the first ring (q[1] to q[6]) cyclically
    for (int i = 1; i <= 6; i++) {
        cnot(q[i], q[(i % 6) + 1]); // (i % 6) + 1 ensures cyclic connection from q[6] to q[1]
    }

    // Entangle qubits in the second ring (q[7] to q[18]) cyclically
    for (int i = 7; i < 18; i++) {
        cnot(q[i], q[i + 1]);
    }
    cnot(q[18], q[7]); // Complete the cycle for the last connection

    // Log the symbolic event for traceability, using the central qubit's ID and tag.
    log_symbolic_event("FLOWER", q[0]->id, q[0]->tag, "Flower of Life pattern entangled");
    return 0; // Success
}

#else // __KERNEL__

/**
 * SYSCALL_DEFINE2(nymya_3353_flower_of_life) - Kernel syscall for Flower of Life entanglement operation.
 * @user_q_array: User-space pointer to an array of user-space qubit pointers.
 * (i.e., `struct nymya_qubit __user *[]`)
 * @count: The total number of qubits in the array provided by user space.
 *
 * This syscall copies the array of user-space qubit pointers, then copies each
 * individual qubit structure from user space to kernel space. It applies the
 * Flower of Life entanglement logic using kernel-space functions (Hadamard and CNOT)
 * to 19 qubits (if available), and finally copies the modified qubit data back to user space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if the user_q_array pointer is NULL or `count` is less than 19,
 * or if any individual user qubit pointer within the unit is NULL.
 * - -EFAULT if copying data to/from user space fails for any qubit or pointer.
 * - -ENOMEM if kernel memory allocation fails.
 * - Error code from underlying gate operations (e.g., nymya_3308_hadamard_gate).
 */
SYSCALL_DEFINE2(nymya_3353_flower_of_life,
    struct nymya_qubit __user * __user *, user_q_array,
    size_t, count) {

    struct nymya_qubit **k_qubits = NULL; // Pointer to array of kernel-space qubit pointers
    struct nymya_qubit __user **user_qubit_ptrs = NULL; // Pointer to array of user-space qubit pointers
    int ret = 0; // Return value for syscall and gate operations
    size_t i; // Loop counter
    const size_t required_qubits = 19; // Minimum qubits for one Flower of Life unit

    // 1. Basic validation: Check for null array pointer and minimum count
    if (!user_q_array || count < required_qubits) {
        pr_err("nymya_3353_flower_of_life: Invalid user_q_array or count (%zu). Minimum %zu qubits required.\n", count, required_qubits);
        return -EINVAL;
    }

    // 2. Allocate kernel memory for the array of user-space qubit pointers
    user_qubit_ptrs = kmalloc_array(count, sizeof(struct nymya_qubit __user *), GFP_KERNEL);
    if (!user_qubit_ptrs) {
        pr_err("nymya_3353_flower_of_life: Failed to allocate memory for user_qubit_ptrs array\n");
        return -ENOMEM;
    }

    // Copy the array of user-space qubit pointers from user space
    if (copy_from_user(user_qubit_ptrs, user_q_array, count * sizeof(struct nymya_qubit __user *))) {
        pr_err("nymya_3353_flower_of_life: Failed to copy user qubit pointers array\n");
        ret = -EFAULT;
        goto cleanup_user_ptrs_array;
    }

    // 3. Allocate kernel memory for the array of kernel-space qubit structures
    k_qubits = kmalloc_array(count, sizeof(struct nymya_qubit *), GFP_KERNEL);
    if (!k_qubits) {
        pr_err("nymya_3353_flower_of_life: Failed to allocate memory for k_qubits array\n");
        ret = -ENOMEM;
        goto cleanup_user_ptrs_array;
    }

    // Initialize k_qubits elements to NULL for safe cleanup
    for (i = 0; i < count; i++) {
        k_qubits[i] = NULL;
    }

    // 4. Allocate memory for each individual qubit and copy data from user space
    for (i = 0; i < count; i++) {
        // Check if individual user-space qubit pointer is NULL
        if (!user_qubit_ptrs[i]) {
            pr_err("nymya_3353_flower_of_life: Null individual user qubit pointer at index %zu\n", i);
            ret = -EINVAL;
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }

        // Allocate memory for the kernel-space copy of the qubit
        k_qubits[i] = kmalloc(sizeof(struct nymya_qubit), GFP_KERNEL);
        if (!k_qubits[i]) {
            pr_err("nymya_3353_flower_of_life: Failed to allocate memory for k_qubit[%zu]\n", i);
            ret = -ENOMEM; // Out of memory
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }

        // Copy the actual qubit data from user space into the allocated kernel memory
        if (copy_from_user(k_qubits[i], user_qubit_ptrs[i], sizeof(struct nymya_qubit))) {
            pr_err("nymya_3353_flower_of_life: Failed to copy k_qubit[%zu] data from user\n", i);
            ret = -EFAULT; // Bad address
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }
    }

    // 5. Apply the Flower of Life entanglement logic for kernel space
    // Apply Hadamard to all 19 qubits (or up to 'count' if less than 19)
    for (i = 0; i < required_qubits; i++) {
        ret = nymya_3308_hadamard_gate(k_qubits[i]);
        if (ret) {
            pr_err("nymya_3353_flower_of_life: Hadamard on q[%zu] failed, error %d\n", i, ret);
            goto cleanup_k_qubits;
        }
    }

    // Entangle the central qubit (k_qubits[0]) with all other 18 qubits
    for (i = 1; i < required_qubits; i++) {
        ret = nymya_3309_controlled_not(k_qubits[0], k_qubits[i]);
        if (ret) {
            pr_err("nymya_3353_flower_of_life: CNOT(q[0], q[%zu]) failed, error %d\n", i, ret);
            goto cleanup_k_qubits;
        }
    }

    // Entangle qubits in the first ring (k_qubits[1] to k_qubits[6]) cyclically
    for (int j = 1; j <= 6; j++) {
        // Ensure indices are within bounds for the ring (1 to 6)
        size_t current_idx = j;
        size_t next_idx = (j % 6) + 1; // 1->2, 2->3, ..., 6->1

        if (current_idx >= count || next_idx >= count) { // Defensive check
            pr_err("nymya_3353_flower_of_life: Ring 1 CNOT indices out of bounds: %zu, %zu\n", current_idx, next_idx);
            ret = -EINVAL;
            goto cleanup_k_qubits;
        }
        ret = nymya_3309_controlled_not(k_qubits[current_idx], k_qubits[next_idx]);
        if (ret) {
            pr_err("nymya_3353_flower_of_life: CNOT(q[%zu], q[%zu]) failed, error %d\n", current_idx, next_idx, ret);
            goto cleanup_k_qubits;
        }
    }

    // Entangle qubits in the second ring (k_qubits[7] to k_qubits[18]) cyclically
    for (int j = 7; j < 18; j++) {
        size_t current_idx = j;
        size_t next_idx = j + 1;

        if (current_idx >= count || next_idx >= count) { // Defensive check
            pr_err("nymya_3353_flower_of_life: Ring 2 CNOT indices out of bounds: %zu, %zu\n", current_idx, next_idx);
            ret = -EINVAL;
            goto cleanup_k_qubits;
        }
        ret = nymya_3309_controlled_not(k_qubits[current_idx], k_qubits[next_idx]);
        if (ret) {
            pr_err("nymya_3353_flower_of_life: CNOT(q[%zu], q[%zu]) failed, error %d\n", current_idx, next_idx, ret);
            goto cleanup_k_qubits;
        }
    }
    // Complete the cycle for the last connection: q[18] to q[7]
    if (18 >= count || 7 >= count) { // Defensive check
        pr_err("nymya_3353_flower_of_life: Last Ring 2 CNOT indices out of bounds: 18, 7\n");
        ret = -EINVAL;
        goto cleanup_k_qubits;
    }
    ret = nymya_3309_controlled_not(k_qubits[18], k_qubits[7]);
    if (ret) {
        pr_err("nymya_3353_flower_of_life: CNOT(q[18], q[7]) failed, error %d\n", ret);
        goto cleanup_k_qubits;
    }


    // 6. Log the symbolic event for traceability
    log_symbolic_event("FLOWER", k_qubits[0]->id, k_qubits[0]->tag, "Flower of Life pattern entangled");

    // 7. Copy the modified qubits back to user space
    // Only proceed if no errors occurred during gate applications
    if (ret == 0) {
        for (i = 0; i < count; i++) {
            // Copy the actual qubit data from kernel memory back to user space
            if (copy_to_user(user_qubit_ptrs[i], k_qubits[i], sizeof(struct nymya_qubit))) {
                pr_err("nymya_3353_flower_of_life: Failed to copy k_qubit[%zu] to user\n", i);
                // Set ret to -EFAULT if any copy fails, but continue to free memory
                if (ret == 0) ret = -EFAULT; // Preserve first error
            }
        }
    }

cleanup_k_qubits:
    // 8. Free all allocated kernel memory for individual qubits
    for (size_t j = 0; j < count; j++) {
        if (k_qubits && k_qubits[j]) { // Check if k_qubits array pointer is valid and individual pointer is valid
            kfree(k_qubits[j]);
            k_qubits[j] = NULL;
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

