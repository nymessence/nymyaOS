// src/nymya_3350_tessellated_hexagons.c
//
// This file implements the nymya_3350_tessellated_hexagons syscall, which
// applies operations to a set of qubits arranged in a tessellated pattern of
// hexagons. This function processes qubits in groups of six, applying a
// specific entangling sequence (Hadamard and CNOTs) to each hexagon.
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
#define __NR_nymya_3350_tessellated_hexagons NYMYA_TESS_HEXAGONS_CODE

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
 * nymya_3350_tessellated_hexagons - Applies operations across a tessellated pattern of hexagons (userland).
 * @q: An array of pointers to nymya_qubit objects.
 * @count: The total number of qubits in the array.
 *
 * This function processes the provided qubits in groups of six, forming
 * "hexagons". For each complete hexagon (six qubits), it applies a
 * sequence of gates: Hadamard on each qubit within the hexagon, then CNOT
 * gates between adjacent qubits in a cyclic manner. This simulates
 * entanglement patterns in a hexagonal lattice. The function only processes
 * full hexagons, so any remaining qubits (if count is not a multiple of 6)
 * are ignored.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit array is NULL or if `count` is less than 6 (cannot form a hexagon).
 * - -1 if any individual qubit pointer within a processed hexagon is NULL.
 */
int nymya_3350_tessellated_hexagons(nymya_qubit* q[], size_t count) {
    // Basic null pointer and minimum count check
    if (!q || count < 6) return -1;

    // Calculate the number of complete hexagons that can be formed
    size_t groups = count / 6;

    // Iterate through each hexagon group
    for (size_t g = 0; g < groups; g++) {
        size_t base = 6 * g; // Base index for the current hexagon's qubits

        // Check for null pointers within the current hexagon and apply Hadamard
        for (int i = 0; i < 6; i++) {
            if (!q[base + i]) return -1; // Return error if any qubit in the hexagon is NULL
            hadamard(q[base + i]);   // Hadamard on each qubit in the hexagon
        }

        // Apply CNOT gates between adjacent qubits in a cyclic manner
        for (int i = 0; i < 6; i++) {
            cnot(q[base + i], q[base + (i + 1) % 6]);
        }

        // Log the symbolic event for traceability, using the first qubit of the hexagon
        log_symbolic_event("HEX_TESS", q[base]->id, q[base]->tag, "Hexagon ring entangle");
    }
    return 0; // Success
}

#else // __KERNEL__

/**
 * SYSCALL_DEFINE2(nymya_3350_tessellated_hexagons) - Kernel syscall for tessellated hexagons operation.
 * @user_q_array: User-space pointer to an array of user-space qubit pointers.
 * (i.e., `struct nymya_qubit __user *[]`)
 * @count: The total number of qubits in the array provided by user space.
 *
 * This syscall copies the array of user-space qubit pointers, then copies each
 * individual qubit structure from user space to kernel space. It applies the
 * tessellated hexagonal lattice gate logic using kernel-space functions
 * (Hadamard and CNOT) to groups of six qubits, and finally copies the
 * modified qubit data back to user space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if the user_q_array pointer is NULL or `count` is less than 6,
 * or if any individual user qubit pointer within a hexagon is NULL.
 * - -EFAULT if copying data to/from user space fails for any qubit or pointer.
 * - -ENOMEM if kernel memory allocation fails.
 * - Error code from underlying gate operations (e.g., nymya_3308_hadamard_gate).
 */
SYSCALL_DEFINE2(nymya_3350_tessellated_hexagons,
    struct nymya_qubit __user * __user *, user_q_array,
    size_t, count) {

    struct nymya_qubit **k_qubits = NULL; // Array of pointers to kernel-space qubit data
    struct nymya_qubit __user **user_qubit_ptrs = NULL; // Array to hold user-space pointers
    int ret = 0; // Return value for syscall and gate operations
    size_t i; // Loop counter
    size_t groups; // Number of complete hexagons

    // 1. Basic validation: Check for null array pointer and minimum count
    if (!user_q_array || count < 6) {
        pr_err("nymya_3350_tessellated_hexagons: Invalid user_q_array or count (%zu)\n", count);
        return -EINVAL;
    }

    // Calculate the number of complete hexagons
    groups = count / 6;

    // 2. Allocate kernel memory for the array of user-space qubit pointers
    user_qubit_ptrs = kmalloc_array(count, sizeof(struct nymya_qubit __user *), GFP_KERNEL);
    if (!user_qubit_ptrs) {
        pr_err("nymya_3350_tessellated_hexagons: Failed to allocate memory for user_qubit_ptrs array\n");
        return -ENOMEM;
    }

    // Copy the array of user-space qubit pointers from user space
    if (copy_from_user(user_qubit_ptrs, user_q_array, count * sizeof(struct nymya_qubit __user *))) {
        pr_err("nymya_3350_tessellated_hexagons: Failed to copy user qubit pointers array\n");
        ret = -EFAULT;
        goto cleanup_user_ptrs_array;
    }

    // 3. Allocate kernel memory for the array of kernel-space qubit structures
    k_qubits = kmalloc_array(count, sizeof(struct nymya_qubit *), GFP_KERNEL);
    if (!k_qubits) {
        pr_err("nymya_3350_tessellated_hexagons: Failed to allocate memory for k_qubits array\n");
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
            pr_err("nymya_3350_tessellated_hexagons: Null individual user qubit pointer at index %zu\n", i);
            ret = -EINVAL;
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }

        // Allocate memory for the kernel-space copy of the qubit
        k_qubits[i] = kmalloc(sizeof(struct nymya_qubit), GFP_KERNEL);
        if (!k_qubits[i]) {
            pr_err("nymya_3350_tessellated_hexagons: Failed to allocate memory for k_qubit[%zu]\n", i);
            ret = -ENOMEM; // Out of memory
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }

        // Copy the actual qubit data from user space into the allocated kernel memory
        if (copy_from_user(k_qubits[i], user_qubit_ptrs[i], sizeof(struct nymya_qubit))) {
            pr_err("nymya_3350_tessellated_hexagons: Failed to copy k_qubit[%zu] data from user\n", i);
            ret = -EFAULT; // Bad address
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }
    }

    // 5. Apply the tessellated hexagons logic for kernel space
    // Iterate through each hexagon group
    for (size_t g = 0; g < groups; g++) {
        size_t base = 6 * g; // Base index for the current hexagon's qubits

        // Apply Hadamard gate to each qubit in the hexagon
        for (int j = 0; j < 6; j++) {
            ret = nymya_3308_hadamard_gate(k_qubits[base + j]);
            if (ret) {
                pr_err("nymya_3350_tessellated_hexagons: Hadamard on hexagon %zu, qubit %d failed, error %d\n", g, j, ret);
                goto cleanup_k_qubits;
            }
        }

        // Apply CNOT gates between adjacent qubits in a cyclic manner
        for (int j = 0; j < 6; j++) {
            ret = nymya_3309_controlled_not(k_qubits[base + j], k_qubits[base + (j + 1) % 6]);
            if (ret) {
                pr_err("nymya_3350_tessellated_hexagons: CNOT(q[%zu+%d], q[%zu+%d]) on hexagon %zu failed, error %d\n", base, j, base, (j + 1) % 6, g, ret);
                goto cleanup_k_qubits;
            }
        }

        // Log the symbolic event for traceability
        log_symbolic_event("HEX_TESS", k_qubits[base]->id, k_qubits[base]->tag, "Hexagon ring entangle");
    }

    // 6. Copy the modified qubits back to user space
    // Only proceed if no errors occurred during gate applications
    if (ret == 0) {
        for (i = 0; i < count; i++) {
            // Copy the actual qubit data from kernel memory back to user space
            if (copy_to_user(user_qubit_ptrs[i], k_qubits[i], sizeof(struct nymya_qubit))) {
                pr_err("nymya_3350_tessellated_hexagons: Failed to copy k_qubit[%zu] to user\n", i);
                // Set ret to -EFAULT if any copy fails, but continue to free memory
                if (ret == 0) ret = -EFAULT;
            }
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

