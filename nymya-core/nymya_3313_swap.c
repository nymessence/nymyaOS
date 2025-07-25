// src/nymya_3313_swap.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_nymya_3313_swap NYMYA_SWAP_CODE

    #include <stdio.h>
    #include <stdlib.h>
    #include <complex.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/module.h> // ADDED: Required for EXPORT_SYMBOL_GPL
#endif

/**
 * nymya_3313_swap - Swap amplitudes of two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function swaps the quantum amplitudes between q1 and q2.
 * Returns 0 on success, or -1 if either pointer is NULL.
 */
#ifndef __KERNEL__

int nymya_3313_swap(nymya_qubit* q1, nymya_qubit* q2) {
    if (!q1 || !q2) return -1;

    _Complex double tmp = q1->amplitude;
    q1->amplitude = q2->amplitude;
    q2->amplitude = tmp;

    log_symbolic_event("SWAP", q1->id, q1->tag, "Swapped with pair");
    return 0;
}

#else // __KERNEL__

/**
 * nymya_3313_swap - Core kernel function to swap amplitudes of two qubits.
 * @k_q1: Pointer to the first kernel-space nymya_qubit struct.
 * @k_q2: Pointer to the second kernel-space nymya_qubit struct.
 *
 * This function swaps the quantum amplitudes between k_q1 and k_q2.
 * This function is designed to be called directly by other kernel code.
 *
 * Returns:
 * 0 on success,
 * -EINVAL if any qubit pointer is NULL.
 */
int nymya_3313_swap(struct nymya_qubit *k_q1, struct nymya_qubit *k_q2) {
    complex_double tmp;

    if (!k_q1 || !k_q2) {
        pr_err("NYMYA: nymya_3313_swap received NULL kernel qubit pointer(s)\n");
        return -EINVAL;
    }

    tmp = k_q1->amplitude;
    k_q1->amplitude = k_q2->amplitude;
    k_q2->amplitude = tmp;

    log_symbolic_event("SWAP", k_q1->id, k_q1->tag, "Swapped with pair");
    return 0;
}
// Export the symbol for this function so other kernel modules/code can call it directly.
EXPORT_SYMBOL_GPL(nymya_3313_swap);


/**
 * SYSCALL_DEFINE2(nymya_3313_swap) - Swap amplitudes of two qubits in kernel space.
 * @user_q1: User-space pointer to the first nymya_qubit struct.
 * @user_q2: User-space pointer to the second nymya_qubit struct.
 *
 * Copies the qubit structs from user space, calls the core swap function,
 * and copies the results back. Logs the swap event referencing the first qubit.
 *
 * Returns:
 * 0 on success,
 * -EINVAL if any user pointer is NULL,
 * -EFAULT if copy_from_user or copy_to_user fails.
 */
SYSCALL_DEFINE2(nymya_3313_swap,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2;
    int ret;

    if (!user_q1 || !user_q2)
        return -EINVAL;

    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;

    // Call the core logic function
    ret = nymya_3313_swap(&k_q1, &k_q2);

    if (ret) // Propagate error from core function
        return ret;

    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return 0;
}

#endif

