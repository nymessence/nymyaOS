// src/nymya_3314_imaginary_swap.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h> // For _Complex double in userland
#define __NR_nymya_3314_imaginary_swap NYMYA_IMSWAP_CODE

/**
 * nymya_3314_imaginary_swap - Swap amplitudes of two qubits with imaginary multiplication (Userland).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * Swaps the amplitudes of q1 and q2, each multiplied by the imaginary unit I.
 * This applies an imaginary phase mirror to the amplitudes during the swap.
 *
 * Returns:
 * 0 on success,
 * -1 if either pointer is NULL.
 */
int nymya_3314_imaginary_swap(nymya_qubit* q1, nymya_qubit* q2) {
    if (!q1 || !q2) return -1;

    _Complex double tmp = q1->amplitude * I;
    q1->amplitude = q2->amplitude * I;
    q2->amplitude = tmp;

    log_symbolic_event("IMSWAP", q1->id, q1->tag, "Imaginary mirror swap");
    return 0;
}

#else // __KERNEL__
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/module.h> // Required for EXPORT_SYMBOL_GPL
#include <linux/printk.h> // Required for pr_err

/**
 * @brief Performs the imaginary amplitude swap on two kernel-space qubits.
 *
 * This function swaps the complex amplitudes of two nymya_qubit structures
 * and applies an imaginary multiplication (rotation by 90 degrees) to each
 * amplitude during the swap.
 *
 * For a complex number (a + bi), multiplication by I results in (-b + ai).
 *
 * @param kq1 Pointer to the first nymya_qubit in kernel space.
 * @param kq2 Pointer to the second nymya_qubit in kernel space.
 * @return 0 on success, -EINVAL if either qubit pointer is NULL.
 */
int nymya_3314_imaginary_swap(struct nymya_qubit *kq1, struct nymya_qubit *kq2) {
    complex_double tmp;

    if (!kq1 || !kq2) {
        pr_err("NYMYA: nymya_3314_imaginary_swap received NULL qubit pointer(s)\n");
        return -EINVAL;
    }

    /*
     * Perform multiplication by I (imaginary unit) on kq1->amplitude:
     * (a + bi) * I = -b + ai
     */
    tmp.re = -kq1->amplitude.im;
    tmp.im =  kq1->amplitude.re;

    /*
     * Similarly multiply kq2->amplitude by I and assign to kq1->amplitude
     */
    complex_double tmp_kq1;
    tmp_kq1.re = -kq2->amplitude.im;
    tmp_kq1.im =  kq2->amplitude.re;

    // Assign swapped and rotated amplitudes
    kq1->amplitude = tmp_kq1;
    kq2->amplitude = tmp;

    // Log the swap event referencing the first qubit
    log_symbolic_event("IMSWAP", kq1->id, kq1->tag, "Imaginary mirror swap");

    return 0; // Always succeeds if given valid kernel-space pointers.
}
EXPORT_SYMBOL_GPL(nymya_3314_imaginary_swap);


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
 * 0 on success,
 * -EINVAL if either user pointer is NULL,
 * -EFAULT if copy_from_user or copy_to_user fails.
 */
SYSCALL_DEFINE2(nymya_3314_imaginary_swap,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2;
    int ret;

    // Validate user pointers
    if (!user_q1 || !user_q2)
        return -EINVAL;

    // Copy data from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;

    // Call the extracted core logic function
    ret = nymya_3314_imaginary_swap(&k_q1, &k_q2);
    if (ret) {
        // Propagate error from the core function if any
        return ret;
    }

    // Copy the modified structs back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;

    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;

    return 0;
}

#endif // __KERNEL__

