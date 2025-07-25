// src/nymya_3352_e8_group.c
//
// This file implements the nymya_3352_e8_group syscall, which applies
// operations to eight qubits, potentially simulating entanglement patterns
// related to the E8 Lie group. This involves applying Hadamard gates to all
// qubits and then CNOT gates between all unique pairs of qubits to create
// a highly entangled state.
//
// The implementation includes both userland and kernel-space versions,
// ensuring proper header inclusion, memory management, and fixed-point
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
 * nymya_3352_e8_group - Applies operations for E8 group-related entanglement to eight qubits (userland).
 * @q: An array of eight pointers to nymya_qubit objects.
 *
 * This function simulates an E8 group-related entanglement pattern. It applies
 * Hadamard gates to all eight qubits. Then, it applies CNOT gates between
 * every unique pair of qubits (i, j) where i < j, followed by CNOT(j, i).
 * This creates a highly entangled state among all eight qubits.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit array is NULL or if any individual qubit pointer is NULL.
 */
int nymya_3352_e8_group(nymya_qubit* q[8]) {
    // Basic null pointer checks for each qubit in the array
    for (int i = 0; i < 8; i++) {
        if (!q[i]) return -1;
    }

    // Apply Hadamard gate to each qubit
    for (int i = 0; i < 8; i++) {
        hadamard(q[i]);
    }

    // Apply CNOT gates between all unique pairs of qubits (i, j)
    // and then CNOT(j, i) to create full entanglement.
    for (int i = 0; i < 8; i++) {
        for (int j = i + 1; j < 8; j++) {
            cnot(q[i], q[j]); // CNOT with q[i] as control, q[j] as target
            cnot(q[j], q[i]); // CNOT with q[j] as control, q[i] as target
        }
    }

    // Log the symbolic event for traceability, using the first qubit's ID and tag.
    log_symbolic_event("E8_GROUP", q[0]->id, q[0]->tag, "E8 8-node full entanglement");
    return 0; // Success
}

#else // __KERNEL__

/**
 * SYSCALL_DEFINE1(nymya_3352_e8_group) - Kernel syscall for E8 group entanglement operation.
 * @user_q_array: User-space pointer to an array of 8 user-space qubit pointers.
 * (i.e., `struct nymya_qubit __user *[8]`)
 *
 * This syscall copies the array of user-space qubit pointers, then copies each
 * individual qubit structure from user space to kernel space. It applies the
 * E8 group entanglement logic using kernel-space functions (Hadamard and CNOT)
 * to eight qubits, and finally copies the modified qubit data back to user space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if the user_q_array pointer is NULL or if any individual user qubit pointer is NULL.
 * - -EFAULT if copying data to/from user space fails for any qubit or pointer.
 * - -ENOMEM if kernel memory allocation fails.
 * - Error code from underlying gate operations (e.g., nymya_3308_hadamard_gate).
 */
SYSCALL_DEFINE1(nymya_3352_e8_group,
    struct nymya_qubit __user * __user *, user_q_array) {

    struct nymya_qubit **k_qubits = NULL; // Changed to pointer to pointer, initialized to NULL
    struct nymya_qubit __user **user_qubit_ptrs = NULL; // Changed to pointer to pointer, initialized to NULL
    int ret = 0; // Return value for syscall and gate operations
    int i, j; // Loop counters
    const size_t num_qubits = 8;

    // 1. Basic validation: Check for null array pointer
    if (!user_q_array) {
        pr_err("nymya_3352_e8_group: Null user_q_array pointer\n");
        return -EINVAL;
    }

    // 2. Allocate kernel memory for the array of user-space qubit pointers
    // This array will hold the addresses of the user-space qubit structures.
    user_qubit_ptrs = kmalloc_array(num_qubits, sizeof(struct nymya_qubit __user *), GFP_KERNEL);
    if (!user_qubit_ptrs) {
        pr_err("nymya_3352_e8_group: Failed to allocate memory for user_qubit_ptrs array\n");
        return -ENOMEM;
    }

    // Copy the array of user-space qubit pointers from user space
    if (copy_from_user(user_qubit_ptrs, user_q_array, num_qubits * sizeof(struct nymya_qubit __user *))) {
        pr_err("nymya_3352_e8_group: Failed to copy user qubit pointers array\n");
        ret = -EFAULT;
        goto cleanup_user_ptrs_array;
    }

    // 3. Allocate kernel memory for the array of kernel-space qubit structures
    // This array will hold pointers to the actual kernel-space qubit data.
    k_qubits = kmalloc_array(num_qubits, sizeof(struct nymya_qubit *), GFP_KERNEL);
    if (!k_qubits) {
        pr_err("nymya_3352_e8_group: Failed to allocate memory for k_qubits array\n");
        ret = -ENOMEM;
        goto cleanup_user_ptrs_array;
    }

    // Initialize k_qubits elements to NULL for safe cleanup
    for (i = 0; i < num_qubits; i++) {
        k_qubits[i] = NULL;
    }

    // 4. Allocate memory for each individual qubit and copy data from user space
    for (i = 0; i < num_qubits; i++) {
        // Check if individual user-space qubit pointer is NULL
        if (!user_qubit_ptrs[i]) {
            pr_err("nymya_3352_e8_group: Null individual user qubit pointer at index %d\n", i);
            ret = -EINVAL;
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }

        // Allocate memory for the kernel-space copy of the qubit
        k_qubits[i] = kmalloc(sizeof(struct nymya_qubit), GFP_KERNEL);
        if (!k_qubits[i]) {
            pr_err("nymya_3352_e8_group: Failed to allocate memory for k_qubit[%d]\n", i);
            ret = -ENOMEM; // Out of memory
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }

        // Copy the actual qubit data from user space into the allocated kernel memory
        if (copy_from_user(k_qubits[i], user_qubit_ptrs[i], sizeof(struct nymya_qubit))) {
            pr_err("nymya_3352_e8_group: Failed to copy k_qubit[%d] data from user\n", i);
            ret = -EFAULT; // Bad address
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }
    }

    // 5. Apply the E8 group entanglement logic for kernel space
    // Apply Hadamard gate to each qubit
    for (i = 0; i < num_qubits; i++) {
        ret = nymya_3308_hadamard_gate(k_qubits[i]);
        if (ret) {
            pr_err("nymya_3352_e8_group: Hadamard on q[%d] failed, error %d\n", i, ret);
            goto cleanup_k_qubits;
        }
    }

    // Apply CNOT gates between all unique pairs of qubits (i, j)
    // and then CNOT(j, i) to create full entanglement.
    for (i = 0; i < num_qubits; i++) {
        for (j = i + 1; j < num_qubits; j++) {
            ret = nymya_3309_controlled_not(k_qubits[i], k_qubits[j]); // CNOT(q[i], q[j])
            if (ret) {
                pr_err("nymya_3352_e8_group: CNOT(q[%d], q[%d]) failed, error %d\n", i, j, ret);
                goto cleanup_k_qubits;
            }
            ret = nymya_3309_controlled_not(k_qubits[j], k_qubits[i]); // CNOT(q[j], q[i])
            if (ret) {
                pr_err("nymya_3352_e8_group: CNOT(q[%d], q[%d]) failed, error %d\n", j, i, ret);
                goto cleanup_k_qubits;
            }
        }
    }

    // 6. Log the symbolic event for traceability
    log_symbolic_event("E8_GROUP", k_qubits[0]->id, k_qubits[0]->tag, "E8 8-node full entanglement");

    // 7. Copy the modified qubits back to user space
    // Only proceed if no errors occurred during gate applications
    if (ret == 0) {
        for (i = 0; i < num_qubits; i++) {
            // Copy the actual qubit data from kernel memory back to user space
            if (copy_to_user(user_qubit_ptrs[i], k_qubits[i], sizeof(struct nymya_qubit))) {
                pr_err("nymya_3352_e8_group: Failed to copy k_qubit[%d] to user\n", i);
                // Set ret to -EFAULT if any copy fails, but continue to free memory
                if (ret == 0) ret = -EFAULT; // Preserve first error
            }
        }
    }

cleanup_k_qubits:
    // 8. Free all allocated kernel memory for individual qubits
    for (j = 0; j < num_qubits; j++) {
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

