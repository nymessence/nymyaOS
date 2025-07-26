// src/nymya_3342_deutsch.c
//
// This file implements the Deutsch algorithm extension for Nymya syscalls.
// The core function applies Hadamard gates around a user-supplied oracle function.

#include "nymya.h" // Declarations for nymya_qubit, nymya_3308_hadamard_gate, log_symbolic_event, and core prototype

#ifndef __KERNEL__
#include <errno.h>
#include <stdio.h>

/**
 * nymya_3342_deutsch - Core Deutsch algorithm (userland)
 * @q1: first qubit (input/output)
 * @q2: second qubit (oracle target)
 * @f:  oracle function to apply (signature void f(nymya_qubit*))
 *
 * Implements: H(q1); f(q2); H(q1);
 * Returns 0 on success, -1 if q1 or q2 is NULL, -EINVAL if f is NULL.
 */
int nymya_3342_deutsch(nymya_qubit* q1, nymya_qubit* q2, void (*f)(nymya_qubit*)) {
    if (!q1 || !q2)
        return -1;
    if (!f)
        return -EINVAL;

    nymya_3308_hadamard_gate(q1);
    f(q2);
    nymya_3308_hadamard_gate(q1);
    log_symbolic_event("DEUTSCH", q1->id, q1->tag, "Deutsch gate applied");
    return 0;
}

#else // __KERNEL__
#include <linux/errno.h>
#include <linux/printk.h>
#include <linux/module.h>

/**
 * nymya_3342_deutsch - Core Deutsch algorithm (kernel)
 * @q1: first qubit
 * @q2: second qubit (oracle target)
 * @f:  oracle function pointer (void f(struct nymya_qubit *))
 *
 * Returns 0 on success, -EINVAL for NULL pointers.
 */
int nymya_3342_deutsch(struct nymya_qubit *q1, struct nymya_qubit *q2, void (*f)(struct nymya_qubit *)) {
    if (!q1 || !q2)
        return -EINVAL;
    if (!f) {
        pr_err("nymya_3342_deutsch: null oracle pointer\n");
        return -EINVAL;
    }

    nymya_3308_hadamard_gate(q1);
    f(q2);
    nymya_3308_hadamard_gate(q1);
    log_symbolic_event("DEUTSCH", q1->id, q1->tag, "Deutsch gate applied");
    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3342_deutsch);

#endif // __KERNEL__

