// src/nymya_3301_identity_gate.c

#include "nymya.h"

#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/types.h>

/*
 * Syscall: nymya_3301_identity_gate
 * Purpose: Apply the quantum identity gate (I) to a qubit.
 * Parameters:
 *     @user_q: Pointer to user-space `nymya_qubit` structure.
 * Behavior:
 *     - Reads the qubit structure from user space.
 *     - Logs a symbolic event to indicate no-op (identity) transformation.
 * Notes:
 *     - This is a symbolic operation only; does not alter amplitude.
 *     - Useful for preserving causality or marking a moment in symbolic space.
 * Return:
 *     0 on success, -EINVAL if null, -EFAULT if copy fails.
 */
SYSCALL_DEFINE1(nymya_3301_identity_gate, struct nymya_qubit __user *, user_q)
{
    struct nymya_qubit kq;

    if (!user_q)
        return -EINVAL;

    if (copy_from_user(&kq, user_q, sizeof(struct nymya_qubit)))
        return -EFAULT;

    log_symbolic_event("ID_GATE", kq.id, kq.tag, "State preserved");
    return 0;
}

#else
#include <stdio.h>
#include <stdlib.h>

/*
 * Userspace Fallback: nymya_3301_identity_gate
 * Purpose: Log identity gate operation on symbolic qubit.
 * Parameters:
 *     @q: Pointer to symbolic qubit.
 * Behavior:
 *     - Logs the event with tag and ID.
 *     - Used for userland simulation/testing environments.
 * Return:
 *     0 on success, -1 if input is null.
 */
int nymya_3301_identity_gate(nymya_qubit* q)
{
    if (!q)
        return -1;

    log_symbolic_event("ID_GATE", q->id, q->tag, "State preserved");
    return 0;
}
#endif

