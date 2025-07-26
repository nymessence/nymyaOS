// src/nymya_3352_e8_group.c
//
// This file implements the nymya_3352_e8_group syscall, which applies
// operations to eight qubits, simulating entanglement patterns related to
// the E8 Lie group. The core exports a fixed-size array signature matching
// the header declaration.

#include "nymya.h" // Common definitions like nymya_qubit and gate macros

#ifndef __KERNEL__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

/**
 * nymya_3352_e8_group - Applies E8 group entanglement (userland)
 * @q: array of 8 pointers to nymya_qubit
 *
 * Applies Hadamard to each qubit, then for every pair (i<j):
 *    cnot(q[i],q[j]); cnot(q[j],q[i]);
 * Returns 0 on success, -1 on null pointer.
 */
int nymya_3352_e8_group(nymya_qubit* q[8]) {
    for (int i = 0; i < 8; i++) {
        if (!q[i]) return -1;
        hadamard(q[i]);
    }
    for (int i = 0; i < 8; i++) {
        for (int j = i+1; j < 8; j++) {
            cnot(q[i], q[j]);
            cnot(q[j], q[i]);
        }
    }
    log_symbolic_event("E8_GROUP", q[0]->id, q[0]->tag, "E8 8-node full entanglement");
    return 0;
}

#else // __KERNEL__
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/module.h>

/**
 * nymya_3352_e8_group - Applies E8 group entanglement (kernel)
 * @q: array of 8 pointers to kernel-space nymya_qubit
 *
 * Matches declaration: int nymya_3352_e8_group(nymya_qubit* q[8]);
 */
int nymya_3352_e8_group(nymya_qubit* q[8]) {
    int ret;
    for (int i = 0; i < 8; i++) {
        if (!q[i]) return -EINVAL;
        ret = nymya_3308_hadamard_gate(q[i]);
        if (ret) return ret;
    }
    for (int i = 0; i < 8; i++) {
        for (int j = i+1; j < 8; j++) {
            ret = nymya_3309_controlled_not(q[i], q[j]); if (ret) return ret;
            ret = nymya_3309_controlled_not(q[j], q[i]); if (ret) return ret;
        }
    }
    log_symbolic_event("E8_GROUP", q[0]->id, q[0]->tag, "E8 8-node full entanglement");
    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3352_e8_group);

SYSCALL_DEFINE1(nymya_3352_e8_group,
    nymya_qubit* __user *, user_q)
{
    nymya_qubit* k_q[8];
    int ret;  // return value for core function

    // Copy user pointers
    if (copy_from_user(k_q, user_q, 8 * sizeof(nymya_qubit *)))
        return -EFAULT;

    // Copy structures individually
    for (int i = 0; i < 8; i++) {
        if (!k_q[i])
            return -EINVAL;
        if (copy_from_user(&k_q[i][0], k_q[i], sizeof(nymya_qubit)))
            return -EFAULT;
    }

    // Core logic
    ret = nymya_3352_e8_group(k_q);

    // Copy back modified qubits
    for (int i = 0; i < 8; i++) {
        copy_to_user(k_q[i], &k_q[i][0], sizeof(nymya_qubit));
    }

    return ret;
}

#endif // __KERNEL__ // __KERNEL__

