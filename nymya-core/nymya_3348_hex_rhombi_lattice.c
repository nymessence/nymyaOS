// src/nymya_3348_hex_rhombi_lattice.c
//
// This file implements the nymya_3348_hex_rhombi_lattice syscall, which applies
// operations to seven qubits arranged in a hexagonal-rhombic lattice configuration.
// This typically involves entangling operations between a central qubit and its
// surrounding six qubits, and then between the outer qubits to form rhombi.
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
#define __NR_nymya_3348_hex_rhombi_lattice NYMYA_HEX_RHOMBI_LATTICE_CODE

    #include <stdlib.h>  // Userland: For general utilities
    #include <math.h>    // Userland: For complex math functions (if needed by sub-gates)
    #include <complex.h> // Userland: For _Complex double type (if needed by sub-gates)
#else
    #include <linux/kernel.h>   // Kernel: For pr_err and general kernel functions
    #include <linux/syscalls.h> // Kernel: For SYSCALL_DEFINE macros
    #include <linux/uaccess.h>  // Kernel: For copy_from_user, copy_to_user
    #include <linux/errno.h>    // Kernel: For error codes like -EINVAL, -EFAULT
    #include <linux/printk.h>   // Kernel: For pr_err
    #include <linux/slab.h>     // Kernel: For kmalloc, kfree
#endif

#ifndef __KERNEL__

/**
 * nymya_3348_hex_rhombi_lattice - Applies operations to seven qubits in a hexagonal-rhombic lattice (userland).
 * @q: An array of seven pointers to nymya_qubit objects.
 * q[0] is typically the central qubit.
 * q[1] through q[6] are the surrounding hexagonal qubits.
 *
 * This function simulates a hexagonal-rhombic lattice interaction. It applies
 * Hadamard gates to the outer qubits and entangles the central qubit with
 * each outer qubit using CNOTs. It then creates rhombi structures by applying
 * CNOTs between adjacent outer qubits and connecting them back to the center.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer in the array is NULL (invalid input).
 */
int nymya_3348_hex_rhombi_lattice(nymya_qubit* q[7]) {
    // Basic null pointer checks for each qubit in the array
    for (int i = 0; i < 7; i++) {
        if (!q[i]) return -1;
    }

    // Apply Hadamard to outer qubits (q[1] to q[6]) and entangle central qubit (q[0]) with them
    for (int i = 1; i < 7; i++) {
        hadamard(q[i]);     // Hadamard on outer qubit
        cnot(q[0], q[i]);   // CNOT with central q[0] as control, q[i] as target
    }

    // Create rhombi by entangling adjacent outer qubits and connecting back to center
    // Rhombi are formed by (q[i], q[i+1], q[0]).
    // Loop from q[1]-q[2] up to q[5]-q[6]
    for (int i = 1; i < 6; i++) {
        cnot(q[i], q[i + 1]);   // CNOT between adjacent outer qubits
        cnot(q[i + 1], q[0]);   // CNOT with outer qubit as control, central q[0] as target
    }
    // Complete the cycle for the last rhombus: (q[6], q[1], q[0])
    cnot(q[6], q[1]);       // CNOT between q[6] and q[1]
    cnot(q[1], q[0]);       // CNOT with q[1] as control, central q[0] as target

    // Log the symbolic event for traceability
    // Using the ID and tag of the central qubit for logging purposes.
    log_symbolic_event("HEX_RHOMBI", q[0]->id, q[0]->tag, "Hexagon tessellated into 3 rhombi");
    return 0;
}

#else // __KERNEL__

/**
 * SYSCALL_DEFINE1(nymya_3348_hex_rhombi_lattice) - Kernel syscall for hexagonal-rhombic lattice operation.
 * @user_q_array: User-space pointer to an array of 7 user-space qubit pointers.
 * (i.e., `struct nymya_qubit __user *[7]`)
 *
 * This syscall copies the array of user-space qubit pointers, then copies each
 * individual qubit structure from user space to kernel space. It applies the
 * hexagonal-rhombic lattice gate logic using kernel-space functions (Hadamard and CNOT),
 * and finally copies the modified qubit data back to user space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if the user_q_array pointer is NULL or any individual user qubit pointer is NULL.
 * - -EFAULT if copying data to/from user space fails for any qubit or pointer.
 * - -ENOMEM if kernel memory allocation fails.
 * - Error code from underlying gate operations (e.g., nymya_3308_hadamard_gate).
 */
SYSCALL_DEFINE1(nymya_3348_hex_rhombi_lattice,
    struct nymya_qubit __user * __user *, user_q_array) {

    struct nymya_qubit *k_qubits[7]; // Array of pointers to kernel-space qubit data
    // Array to hold the user-space pointers themselves, copied from user_q_array
    struct nymya_qubit __user *user_qubit_ptrs[7];
    int ret = 0; // Return value for syscall and gate operations
    int i; // Loop counter

    // Initialize k_qubits pointers to NULL for safe cleanup
    for (i = 0; i < 7; i++) {
        k_qubits[i] = NULL;
    }

    // 1. Check for null pointer for the array of pointers itself
    if (!user_q_array) {
        pr_err("nymya_3348_hex_rhombi_lattice: Null user_q_array pointer\n");
        return -EINVAL; // Invalid argument
    }

    // 2. Copy the array of user-space qubit pointers from user space
    // This copies the addresses of the user-space qubits, not the qubit data itself.
    if (copy_from_user(user_qubit_ptrs, user_q_array, sizeof(user_qubit_ptrs))) {
        pr_err("nymya_3348_hex_rhombi_lattice: Failed to copy user qubit pointers array\n");
        ret = -EFAULT;
        goto cleanup_k_qubits;
    }

    // 3. Allocate kernel memory for each qubit and copy individual qubit data from user space
    for (i = 0; i < 7; i++) {
        // Check if individual user-space qubit pointer is NULL
        if (!user_qubit_ptrs[i]) {
            pr_err("nymya_3348_hex_rhombi_lattice: Null individual user qubit pointer at index %d\n", i);
            ret = -EINVAL;
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }

        // Allocate memory for the kernel-space copy of the qubit
        k_qubits[i] = kmalloc(sizeof(struct nymya_qubit), GFP_KERNEL);
        if (!k_qubits[i]) {
            pr_err("nymya_3348_hex_rhombi_lattice: Failed to allocate memory for k_qubit[%d]\n", i);
            ret = -ENOMEM; // Out of memory
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }

        // Copy the actual qubit data from user space into the allocated kernel memory
        if (copy_from_user(k_qubits[i], user_qubit_ptrs[i], sizeof(struct nymya_qubit))) {
            pr_err("nymya_3348_hex_rhombi_lattice: Failed to copy k_qubit[%d] data from user\n", i);
            ret = -EFAULT; // Bad address
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }
    }

    // 4. Apply the hexagonal-rhombic lattice logic for kernel space
    // Apply Hadamard to outer qubits (k_qubits[1] to k_qubits[6]) and entangle central qubit (k_qubits[0]) with them
    for (i = 1; i < 7; i++) {
        ret = nymya_3308_hadamard_gate(k_qubits[i]); // Hadamard on outer qubit
        if (ret) {
            pr_err("nymya_3348_hex_rhombi_lattice: Hadamard on q[%d] failed, error %d\n", i, ret);
            goto cleanup_k_qubits;
        }
        ret = nymya_3309_controlled_not(k_qubits[0], k_qubits[i]); // CNOT(central, outer)
        if (ret) {
            pr_err("nymya_3348_hex_rhombi_lattice: CNOT(q[0], q[%d]) failed, error %d\n", i, ret);
            goto cleanup_k_qubits;
        }
    }

    // Create rhombi by entangling adjacent outer qubits and connecting back to center
    // Rhombi are formed by (q[i], q[i+1], q[0]).
    // Loop from q[1]-q[2] up to q[5]-q[6]
    for (i = 1; i < 6; i++) {
        ret = nymya_3309_controlled_not(k_qubits[i], k_qubits[i + 1]); // CNOT(adjacent outer)
        if (ret) {
            pr_err("nymya_3348_hex_rhombi_lattice: CNOT(q[%d], q[%d]) failed, error %d\n", i, i + 1, ret);
            goto cleanup_k_qubits;
        }
        ret = nymya_3309_controlled_not(k_qubits[i + 1], k_qubits[0]); // CNOT(outer, central)
        if (ret) {
            pr_err("nymya_3348_hex_rhombi_lattice: CNOT(q[%d], q[0]) failed, error %d\n", i + 1, ret);
            goto cleanup_k_qubits;
        }
    }
    // Complete the cycle for the last rhombus: (q[6], q[1], q[0])
    ret = nymya_3309_controlled_not(k_qubits[6], k_qubits[1]); // CNOT(q[6], q[1])
    if (ret) {
        pr_err("nymya_3348_hex_rhombi_lattice: CNOT(q[6], q[1]) failed, error %d\n", ret);
        goto cleanup_k_qubits;
    }
    ret = nymya_3309_controlled_not(k_qubits[1], k_qubits[0]); // CNOT(q[1], q[0])
    if (ret) {
        pr_err("nymya_3348_hex_rhombi_lattice: CNOT(q[1], q[0]) failed, error %d\n", ret);
        goto cleanup_k_qubits;
    }

    // 5. Log the symbolic event for traceability
    // Using the ID and tag of the central qubit for logging purposes.
    log_symbolic_event("HEX_RHOMBI", k_qubits[0]->id, k_qubits[0]->tag, "Hexagon tessellated into 3 rhombi");

    // 6. Copy the modified qubits back to user space
    // All seven qubits can be modified, so all should be copied back.
    for (i = 0; i < 7; i++) {
        if (copy_to_user(user_qubit_ptrs[i], k_qubits[i], sizeof(struct nymya_qubit))) {
            pr_err("nymya_3348_hex_rhombi_lattice: Failed to copy k_qubit[%d] to user\n", i);
            // Set ret to -EFAULT if any copy fails, but continue to free memory
            if (ret == 0) ret = -EFAULT;
        }
    }

cleanup_k_qubits:
    // 7. Free all allocated kernel memory for qubits
    for (int j = 0; j < 7; j++) {
        if (k_qubits[j]) { // Only free if memory was successfully allocated
            kfree(k_qubits[j]);
            k_qubits[j] = NULL; // Prevent double free
        }
    }

    return ret; // Return 0 on success, or error code if any operation failed
}

#endif

