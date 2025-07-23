// src/nymya_3314_imaginary_swap.c

#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
    #include <complex.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
#endif

/**
 * nymya_3314_imaginary_swap - Swap amplitudes of two qubits with imaginary multiplication.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * Swaps the amplitudes of q1 and q2, each multiplied by the imaginary unit I.
 * This applies an imaginary phase mirror to the amplitudes during the swap.
 *
 * Returns:
 *  0 on success,
 * -1 if either pointer is NULL.
 */
#ifndef __KERNEL__

int nymya_3314_imaginary_swap(nymya_qubit* q1, nymya_qubit* q2) {
    if (!q1 || !q2) return -1;

    _Complex double tmp = q1->amplitude * I;
    q1->amplitude = q2->amplitude * I;
    q2->amplitude = tmp;

    log_symbolic_event("IMSWAP", q1->id, q1->tag, "Imaginary mirror swap");
    return 0;
}

#else

/**
 * SYSCALL_DEFINE2(nymya_3314_imaginary_swap) - Kernel syscall for imaginary amplitude swap.
 * @user_q1: User-space pointer to the first nymya_qubit struct.
 * @user_q2: User-space pointer to the second nymya_qubit struct.
 *
 * Copies the qubit structs from user space into kernel space, performs a swap of their amplitudes
 * with each amplitude multiplied by the imaginary unit I (rotated by 90 degrees in the complex plane),
 * and copies the updated structs back to user space.
 *
 * The multiplication by the imaginary unit I is done manually using fixed-point arithmetic:
 * For a complex number (a + bi), multiplication by I results in (-b + ai).
 *
 * Returns:
 *  0 on success,
 * -EINVAL if either user pointer is NULL,
 * -EFAULT if copy_from_user or copy_to_user fails.
 */
SYSCALL_DEFINE2(nymya_3314_imaginary_swap,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2;
    complex_double tmp;

    // Validate user pointers
    if (!user_q1 || !user_q2)
        return -EINVAL;

    // Copy data from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;

    /*
     * Perform multiplication by I (imaginary unit) on k_q1.amplitude:
     * (a + bi) * I = -b + ai
     */
    tmp.re = -k_q1.amplitude.im;
    tmp.im =  k_q1.amplitude.re;

    /*
     * Similarly multiply k_q2.amplitude by I and assign to k_q1.amplitude
     */
    complex_double tmp_q1;
    tmp_q1.re = -k_q2.amplitude.im;
    tmp_q1.im =  k_q2.amplitude.re;

    // Assign swapped and rotated amplitudes
    k_q1.amplitude = tmp_q1;
    k_q2.amplitude = tmp;

    // Log the swap event referencing the first qubit
    log_symbolic_event("IMSWAP", k_q1.id, k_q1.tag, "Imaginary mirror swap");

    // Copy the modified structs back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return 0;
}

#endif

