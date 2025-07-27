// src/nymya_3354_metatron_cube.c
//
// This file implements the nymya_3354_metatron_cube syscall, which applies
// operations to a set of qubits arranged in a pattern inspired by the
// "Metatron's Cube" geometric design. This involves entangling a central
// qubit with surrounding ones and then creating connections between specific
// outer qubits to form a complex entangled structure.
//
// The implementation includes both userland and kernel-space versions,
// ensuring proper header inclusion, robust memory management, and fixed-point
// arithmetic for kernel operations.

#include "nymya.h" // Common definitions like nymya_qubit, and gate macros

#ifndef __KERNEL__

#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>    // Userland: For standard I/O (e.g., in log_symbolic_event)
#include <stdlib.h>   // Userland: For general utilities (e.g., size_t)
#include <math.h>     // Userland: For complex math functions (if needed by sub-gates)
#include <complex.h>  // Userland: For _Complex double type (if needed by sub-gates)
#define __NR_nymya_3354_metatron_cube NYMYA_METATRON_CUBE_CODE

/**
 * nymya_3354_metatron_cube - Applies operations related to the "Metatron's Cube" pattern to qubits (userland).
 * @q: An array of pointers to nymya_qubit objects.
 * @count: The total number of qubits in the array.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit array is NULL or if `count` is less than 13.
 * - -1 if any individual qubit pointer within the processed unit is NULL.
 */
int nymya_3354_metatron_cube(nymya_qubit* q[], size_t count) {
    if (!q || count < 13) return -1;

    for (size_t i = 0; i < 13; i++) {
        if (!q[i]) return -1;
        hadamard(q[i]);
    }

    for (size_t i = 1; i < 13; i++) {
        cnot(q[0], q[i]);
    }

    for (int i = 1; i <= 6; i++) {
        cnot(q[i], q[i + 6]);
    }

    log_symbolic_event("METATRON", q[0]->id, q[0]->tag,
        "Metatron’s Cube geometry entangled");
    return 0;
}

#else  // __KERNEL__

    int nymya_3354_metatron_cube_core(struct nymya_qubit **k_qubits, size_t count);


#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/module.h>

extern int nymya_3308_hadamard_gate(struct nymya_qubit *q);
extern int nymya_3309_controlled_not(struct nymya_qubit *q_ctrl, struct nymya_qubit *q_target);
extern int log_symbolic_event(const char* gate, uint64_t id, const char* tag, const char* msg);

int nymya_3354_metatron_cube_core(struct nymya_qubit **k_qubits, size_t count) {
    int ret = 0;
    const size_t required_qubits = 13;

    for (size_t i = 0; i < required_qubits; i++) {
        ret = nymya_3308_hadamard_gate(k_qubits[i]);
        if (ret) {
            pr_err("nymya_3354_metatron_cube_core: Hadamard on q[%zu] failed, error %d\n", i, ret);
            return ret;
        }
    }

    for (size_t i = 1; i < required_qubits; i++) {
        ret = nymya_3309_controlled_not(k_qubits[0], k_qubits[i]);
        if (ret) {
            pr_err("nymya_3354_metatron_cube_core: CNOT(q[0], q[%zu]) failed, error %d\n", i, ret);
            return ret;
        }
    }

    for (int j = 1; j <= 6; j++) {
        size_t current_idx = j;
        size_t target_idx = j + 6;
        if (current_idx >= count || target_idx >= count) {
            pr_err("nymya_3354_metatron_cube_core: CNOT index out of bounds: %zu, %zu\n", current_idx, target_idx);
            return -EINVAL;
        }
        ret = nymya_3309_controlled_not(k_qubits[current_idx], k_qubits[target_idx]);
        if (ret) {
            pr_err("nymya_3354_metatron_cube_core: CNOT(q[%zu], q[%zu]) failed, error %d\n", current_idx, target_idx, ret);
            return ret;
        }
    }

    log_symbolic_event("METATRON", k_qubits[0]->id, k_qubits[0]->tag,
        "Metatron’s Cube geometry entangled");

    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3354_metatron_cube_core);

SYSCALL_DEFINE2(nymya_3354_metatron_cube,
    struct nymya_qubit __user * __user *, user_q_array,
    size_t, count) {

    struct nymya_qubit **k_qubits = NULL;
    struct nymya_qubit __user **user_qubit_ptrs = NULL;
    int ret = 0;
    const size_t required_qubits = 13;

    if (!user_q_array || count < required_qubits) {
        pr_err("nymya_3354_metatron_cube: Invalid input\n");
        return -EINVAL;
    }

    user_qubit_ptrs = kmalloc_array(count, sizeof(*user_qubit_ptrs), GFP_KERNEL);
    if (!user_qubit_ptrs) {
        ret = -ENOMEM;
        goto cleanup_user_ptrs_array;
    }

    if (copy_from_user(user_qubit_ptrs, user_q_array, count * sizeof(*user_qubit_ptrs))) {
        ret = -EFAULT;
        goto cleanup_user_ptrs_array;
    }

    k_qubits = kmalloc_array(count, sizeof(*k_qubits), GFP_KERNEL);
    if (!k_qubits) {
        ret = -ENOMEM;
        goto cleanup_user_ptrs_array;
    }

    for (size_t i = 0; i < count; i++) {
        k_qubits[i] = NULL;
    }

    for (size_t i = 0; i < count; i++) {
        if (!user_qubit_ptrs[i]) {
            ret = -EINVAL;
            goto cleanup_k_qubits;
        }

        k_qubits[i] = kmalloc(sizeof(struct nymya_qubit), GFP_KERNEL);
        if (!k_qubits[i]) {
            ret = -ENOMEM;
            goto cleanup_k_qubits;
        }

        if (copy_from_user(k_qubits[i], user_qubit_ptrs[i], sizeof(struct nymya_qubit))) {
            ret = -EFAULT;
            goto cleanup_k_qubits;
        }
    }

    ret = nymya_3354_metatron_cube_core(k_qubits, count);
    if (ret)
        goto cleanup_k_qubits;

    for (size_t i = 0; i < count; i++) {
        if (copy_to_user(user_qubit_ptrs[i], k_qubits[i], sizeof(struct nymya_qubit))) {
            if (ret == 0) ret = -EFAULT;
        }
    }

cleanup_k_qubits:
    for (size_t j = 0; j < count; j++) {
        if (k_qubits && k_qubits[j]) {
            kfree(k_qubits[j]);
            k_qubits[j] = NULL;
        }
    }
    kfree(k_qubits);

cleanup_user_ptrs_array:
    kfree(user_qubit_ptrs);

    return ret;
}

#endif  // __KERNEL__
