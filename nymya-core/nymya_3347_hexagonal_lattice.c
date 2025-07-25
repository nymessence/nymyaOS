// src/nymya_3347_hexagonal_lattice.c
//
// This file implements the nymya_3347_hexagonal_lattice syscall, which applies
// operations to six qubits arranged in a hexagonal lattice configuration. This
// typically involves entangling operations between adjacent qubits in the "hexagon".
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
#define __NR_nymya_3347_hexagonal_lattice NYMYA_HEXAGONAL_LATTICE_CODE

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
 * nymya_3347_hexagonal_lattice - Applies operations to six qubits in a hexagonal lattice (userland).
 * @q: An array of six pointers to nymya_qubit objects.
 *
 * This function simulates a hexagonal lattice interaction by applying a sequence
 * of gates: Hadamard on each qubit, then CNOT gates between adjacent qubits
 * in a cyclic manner (q[i] to q[(i+1)%6]). This creates entanglement across the
 * six qubits forming a hexagonal ring.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer in the array is NULL (invalid input).
 */
int nymya_3347_hexagonal_lattice(nymya_qubit* q[6]) {
    // Basic null pointer checks for each qubit in the array
    for (int i = 0; i < 6; i++) {
        if (!q[i]) return -1;
    }

    // Apply Hadamard gate to each qubit
    for (int i = 0; i < 6; i++) {
        hadamard(q[i]);
    }

    // Apply CNOT gates between adjacent qubits in a cyclic manner
    for (int i = 0; i < 6; i++) {
        cnot(q[i], q[(i + 1) % 6]);
    }

    // Log the symbolic event for traceability
    // Using the ID and tag of the first qubit for logging purposes.
    log_symbolic_event("HEX_LATTICE", q[0]->id, q[0]->tag, "Hexagonal ring lattice formed");
    return 0;
}

#else // __KERNEL__

/**
 * SYSCALL_DEFINE1(nymya_3347_hexagonal_lattice) - Kernel syscall for hexagonal lattice operation.
 * @user_q_array: User-space pointer to an array of 6 user-space qubit pointers.
 * (i.e., `struct nymya_qubit __user *[6]`)
 *
 * This syscall copies the array of user-space qubit pointers, then copies each
 * individual qubit structure from user space to kernel space. It applies the
 * hexagonal lattice gate logic using kernel-space functions (Hadamard and CNOT),
 * and finally copies the modified qubit data back to user space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if the user_q_array pointer is NULL or any individual user qubit pointer is NULL.
 * - -EFAULT if copying data to/from user space fails for any qubit or pointer.
 * - -ENOMEM if kernel memory allocation fails.
 * - Error code from underlying gate operations (e.g., nymya_3308_hadamard_gate).
 */
SYSCALL_DEFINE1(nymya_3347_hexagonal_lattice,
    struct nymya_qubit __user * __user *, user_q_array) {

    struct nymya_qubit *k_qubits[6]; // Array of pointers to kernel-space qubit data
    // Array to hold the user-space pointers themselves, copied from user_q_array
    struct nymya_qubit __user *user_qubit_ptrs[6];
    int ret = 0; // Return value for syscall and gate operations
    int i; // Loop counter

    // Initialize k_qubits pointers to NULL for safe cleanup
    for (i = 0; i < 6; i++) {
        k_qubits[i] = NULL;
    }

    // 1. Check for null pointer for the array of pointers itself
    if (!user_q_array) {
        pr_err("nymya_3347_hexagonal_lattice: Null user_q_array pointer\n");
        return -EINVAL; // Invalid argument
    }

    // 2. Copy the array of user-space qubit pointers from user space
    // This copies the addresses of the user-space qubits, not the qubit data itself.
    if (copy_from_user(user_qubit_ptrs, user_q_array, sizeof(user_qubit_ptrs))) {
        pr_err("nymya_3347_hexagonal_lattice: Failed to copy user qubit pointers array\n");
        return -EFAULT;
    }

    // 3. Allocate kernel memory for each qubit and copy individual qubit data from user space
    for (i = 0; i < 6; i++) {
        // Check if individual user-space qubit pointer is NULL
        if (!user_qubit_ptrs[i]) {
            pr_err("nymya_3347_hexagonal_lattice: Null individual user qubit pointer at index %d\n", i);
            ret = -EINVAL;
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }

        // Allocate memory for the kernel-space copy of the qubit
        k_qubits[i] = kmalloc(sizeof(struct nymya_qubit), GFP_KERNEL);
        if (!k_qubits[i]) {
            pr_err("nymya_3347_hexagonal_lattice: Failed to allocate memory for k_qubit[%d]\n", i);
            ret = -ENOMEM; // Out of memory
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }

        // Copy the actual qubit data from user space into the allocated kernel memory
        if (copy_from_user(k_qubits[i], user_qubit_ptrs[i], sizeof(struct nymya_qubit))) {
            pr_err("nymya_3347_hexagonal_lattice: Failed to copy k_qubit[%d] data from user\n", i);
            ret = -EFAULT; // Bad address
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }
    }

    // 4. Apply the hexagonal lattice logic for kernel space
    // Apply Hadamard gate to each qubit
    for (i = 0; i < 6; i++) {
        ret = nymya_3308_hadamard_gate(k_qubits[i]);
        if (ret) {
            pr_err("nymya_3347_hexagonal_lattice: Hadamard on q[%d] failed, error %d\n", i, ret);
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }
    }

    // Apply CNOT gates between adjacent qubits in a cyclic manner
    for (i = 0; i < 6; i++) {
        ret = nymya_3309_controlled_not(k_qubits[i], k_qubits[(i + 1) % 6]);
        if (ret) {
            pr_err("nymya_3347_hexagonal_lattice: CNOT(q[%d], q[%d]) failed, error %d\n", i, (i + 1) % 6, ret);
            goto cleanup_k_qubits; // Jump to cleanup allocated memory
        }
    }

    // 5. Log the symbolic event for traceability
    // Using the ID and tag of the first qubit for logging purposes.
    log_symbolic_event("HEX_LATTICE", k_qubits[0]->id, k_qubits[0]->tag, "Hexagonal ring lattice formed");

    // 6. Copy the modified qubits back to user space
    // All six qubits can be modified, so all should be copied back.
    for (i = 0; i < 6; i++) {
        if (copy_to_user(user_qubit_ptrs[i], k_qubits[i], sizeof(struct nymya_qubit))) {
            pr_err("nymya_3347_hexagonal_lattice: Failed to copy k_qubit[%d] to user\n", i);
            ret = -EFAULT; // Bad address, but continue to free memory
        }
    }

cleanup_k_qubits:
    // 7. Free all allocated kernel memory for qubits
    for (int j = 0; j < 6; j++) {
        if (k_qubits[j]) { // Only free if memory was successfully allocated
            kfree(k_qubits[j]);
            k_qubits[j] = NULL; // Prevent double free
        }
    }

    return ret; // Return 0 on success, or error code if any operation failed
}

#endif

