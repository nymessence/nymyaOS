// src/nymya_3349_tessellated_triangles.c
//
// This file implements the nymya_3349_tessellated_triangles syscall, which
// applies operations to a set of qubits arranged in a tessellated pattern of
// triangles. This function processes qubits in groups of three, applying a
// specific entangling sequence (Hadamard and CNOTs) to each triangle.
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
 * nymya_3349_tessellated_triangles - Applies operations across a tessellated pattern of triangles (userland).
 * @q: An array of pointers to nymya_qubit objects.
 * @count: The total number of qubits in the array.
 *
 * This function processes the provided qubits in groups of three, forming
 * "triangles". For each complete triangle (three qubits), it applies a
 * sequence of gates: Hadamard on the first qubit, then CNOT(a, b), CNOT(b, c),
 * and CNOT(c, a), where a, b, c are the qubits in the triangle. This simulates
 * entanglement patterns in a triangular lattice. The function only processes
 * full triangles, so any remaining qubits (if count is not a multiple of 3)
 * are ignored.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit array is NULL or if `count` is less than 3 (cannot form a triangle).
 * - -1 if any individual qubit pointer within a processed triangle is NULL.
 */
int nymya_3349_tessellated_triangles(nymya_qubit* q[], size_t count) {
    // Basic null pointer and minimum count check
    if (!q || count < 3) return -1;

    // Calculate the number of complete triangles that can be formed
    size_t groups = count / 3;

    // Iterate through each triangle group
    for (size_t g = 0; g < groups; g++) {
        // Get pointers to the three qubits in the current triangle
        nymya_qubit *a = q[3 * g];
        nymya_qubit *b = q[3 * g + 1];
        nymya_qubit *c = q[3 * g + 2];

        // Check for null pointers within the current triangle
        if (!a || !b || !c) return -1; // Return error if any qubit in the triangle is NULL

        // Apply the gate sequence for a triangular entanglement
        hadamard(a);   // Hadamard on qubit 'a'
        cnot(a, b);    // CNOT with 'a' as control, 'b' as target
        cnot(b, c);    // CNOT with 'b' as control, 'c' as target
        cnot(c, a);    // CNOT with 'c' as control, 'a' as target

        // Log the symbolic event for traceability, using the first qubit of the triangle
        log_symbolic_event("TRI_TESS", a->id, a->tag, "Triangle entangle");
    }
    return 0; // Success
}

#else // __KERNEL__

/**
 * SYSCALL_DEFINE2(nymya_3349_tessellated_triangles) - Kernel syscall for tessellated triangles operation.
 * @user_q_array: User-space pointer to an array of user-space qubit pointers.
 * (i.e., `struct nymya_qubit __user *[]`)
 * @count: The total number of qubits in the array provided by user space.
 *
 * This syscall copies the array of user-space qubit pointers, then copies each
 * individual qubit structure from user space to kernel space. It applies the
 * tessellated triangular lattice gate logic using kernel-space functions
 * (Hadamard and CNOT) to groups of three qubits, and finally copies the
 * modified qubit data back to user space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if the user_q_array pointer is NULL or `count` is less than 3,
 * or if any individual user qubit pointer within a triangle is NULL.
 * - -EFAULT if copying data to/from user space fails for any qubit or pointer.
 * - -ENOMEM if kernel memory allocation fails.
 * - Error code from underlying gate operations (e.g., nymya_3308_hadamard_gate).
 */
SYSCALL_DEFINE2(nymya_3349_tessellated_triangles,
    struct nymya_qubit __user * __user *, user_q_array,
    size_t, count) {

    struct nymya_qubit **k_qubits = NULL; // Array of pointers to kernel-space qubit data
    struct nymya_qubit __user **user_qubit_ptrs = NULL; // Array to hold user-space pointers
    int ret = 0; // Return value for syscall and gate operations
    size_t i; // Loop counter
    size_t groups; // Number of complete triangles

    // 1. Basic validation: Check for null array pointer and minimum count
    if (!user_q_array || count < 3) {
        pr_err("nymya_3349_tessellated_triangles: Invalid user_q_array or count (%zu)\n", count);
        return -EINVAL;
    }

    // Calculate the number of complete triangles
    groups = count / 3;

    // 2. Allocate kernel memory for the array of user-space qubit pointers
    user_qubit_ptrs = kmalloc_array(count, sizeof(struct nymya_qubit __user *), GFP_KERNEL);
    if (!user_qubit_ptrs) {
        pr_err("nymya_3349_tessellated_triangles: Failed to allocate memory for user_qubit_ptrs array\n");
        return -ENOMEM;
    }

    // Copy the array of user-space qubit pointers from user space
    if (copy_from_user(user_qubit_ptrs, user_q_array, count * sizeof(struct nymya_qubit __user *))) {
        pr_err("nymya_3349_tessellated_triangles: Failed to copy user qubit pointers array\n");
        ret = -EFAULT;
        goto cleanup_user_ptrs_array;
    }

    // 3. Allocate kernel memory for the array of kernel-space qubit structures
    k_qubits = kmalloc_array(count, sizeof(struct nymya_qubit *), GFP_KERNEL);
    if (!k_qubits) {
        pr_err("nymya_3349_tessellated_triangles: Failed to allocate memory for k_qubits array\n");
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
            pr_err("nymya_3349_tessellated_triangles: Null individual user qubit pointer at index %zu\n", i);
            ret = -EINVAL;
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }

        // Allocate memory for the kernel-space copy of the qubit
        k_qubits[i] = kmalloc(sizeof(struct nymya_qubit), GFP_KERNEL);
        if (!k_qubits[i]) {
            pr_err("nymya_3349_tessellated_triangles: Failed to allocate memory for k_qubit[%zu]\n", i);
            ret = -ENOMEM; // Out of memory
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }

        // Copy the actual qubit data from user space into the allocated kernel memory
        if (copy_from_user(k_qubits[i], user_qubit_ptrs[i], sizeof(struct nymya_qubit))) {
            pr_err("nymya_3349_tessellated_triangles: Failed to copy k_qubit[%zu] data from user\n", i);
            ret = -EFAULT; // Bad address
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }
    }

    // 5. Apply the tessellated triangles logic for kernel space
    // Iterate through each triangle group
    for (size_t g = 0; g < groups; g++) {
        // Get pointers to the three qubits in the current triangle (kernel-space copies)
        struct nymya_qubit *a = k_qubits[3 * g];
        struct nymya_qubit *b = k_qubits[3 * g + 1];
        struct nymya_qubit *c = k_qubits[3 * g + 2];

        // Apply the gate sequence for a triangular entanglement using kernel functions
        ret = nymya_3308_hadamard_gate(a); // Hadamard on qubit 'a'
        if (ret) {
            pr_err("nymya_3349_tessellated_triangles: Hadamard on triangle %zu, qubit 'a' failed, error %d\n", g, ret);
            goto cleanup_k_qubits;
        }

        ret = nymya_3309_controlled_not(a, b); // CNOT(a, b)
        if (ret) {
            pr_err("nymya_3349_tessellated_triangles: CNOT(a, b) on triangle %zu failed, error %d\n", g, ret);
            goto cleanup_k_qubits;
        }

        ret = nymya_3309_controlled_not(b, c); // CNOT(b, c)
        if (ret) {
            pr_err("nymya_3349_tessellated_triangles: CNOT(b, c) on triangle %zu failed, error %d\n", g, ret);
            goto cleanup_k_qubits;
        }

        ret = nymya_3309_controlled_not(c, a); // CNOT(c, a)
        if (ret) {
            pr_err("nymya_3349_tessellated_triangles: CNOT(c, a) on triangle %zu failed, error %d\n", g, ret);
            goto cleanup_k_qubits;
        }

        // Log the symbolic event for traceability
        log_symbolic_event("TRI_TESS", a->id, a->tag, "Triangle entangle");
    }

    // 6. Copy the modified qubits back to user space
    // Only proceed if no errors occurred during gate applications
    if (ret == 0) {
        for (i = 0; i < count; i++) {
            // Copy the actual qubit data from kernel memory back to user space
            if (copy_to_user(user_qubit_ptrs[i], k_qubits[i], sizeof(struct nymya_qubit))) {
                pr_err("nymya_3349_tessellated_triangles: Failed to copy k_qubit[%zu] to user\n", i);
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

