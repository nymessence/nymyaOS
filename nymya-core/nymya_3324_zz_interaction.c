// src/nymya_3324_zz_interaction.c

#include "nymya.h" // Provides fixed_cos, fixed_sin, and FIXED_POINT_SCALE for kernel

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_nymya_3324_zz_interaction NYMYA_ZZ_CODE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 * nymya_3324_zz_interaction - Applies a ZZ interaction between two qubits (userland).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @theta: Rotation angle in radians (double).
 *
 * This applies the phase e^(i*theta) to both qubit amplitudes.
 */
int nymya_3324_zz_interaction(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    if (!q1 || !q2) return -1;
    _Complex double zphase = cexp(I * theta);
    q1->amplitude *= zphase;
    q2->amplitude *= zphase;
    log_symbolic_event("ZZ", q2->id, q2->tag, "Applied ZZ phase coupling");
    return 0;
}

#else // __KERNEL__
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/printk.h>

/**
 * nymya_3324_zz_interaction - Core ZZ interaction (kernel).
 * @kq1: First kernel-space qubit.
 * @kq2: Second kernel-space qubit.
 * @theta: Angle in fixed-point.
 */
int nymya_3324_zz_interaction(struct nymya_qubit *kq1,
                              struct nymya_qubit *kq2,
                              int64_t theta) {
    int64_t cos_val = fixed_cos(theta);
    int64_t sin_val = fixed_sin(theta);

    // Compute and apply phase for first qubit
    int64_t real1 = (kq1->amplitude.re * cos_val - kq1->amplitude.im * sin_val) / FIXED_POINT_SCALE;
    int64_t imag1 = (kq1->amplitude.re * sin_val + kq1->amplitude.im * cos_val) / FIXED_POINT_SCALE;
    kq1->amplitude.re = real1;
    kq1->amplitude.im = imag1;

    // Compute and apply phase for second qubit
    int64_t real2 = (kq2->amplitude.re * cos_val - kq2->amplitude.im * sin_val) / FIXED_POINT_SCALE;
    int64_t imag2 = (kq2->amplitude.re * sin_val + kq2->amplitude.im * cos_val) / FIXED_POINT_SCALE;
    kq2->amplitude.re = real2;
    kq2->amplitude.im = imag2;

    log_symbolic_event("ZZ", kq2->id, kq2->tag, "Applied ZZ phase coupling");
    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3324_zz_interaction);

/**
 * SYSCALL_DEFINE3(nymya_3324_zz_interaction) - Kernel syscall for ZZ interaction.
 */
SYSCALL_DEFINE3(nymya_3324_zz_interaction,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    int64_t, theta) {

    struct nymya_qubit k1, k2;
    if (!user_q1 || !user_q2) return -EINVAL;
    if (copy_from_user(&k1, user_q1, sizeof(k1))) return -EFAULT;
    if (copy_from_user(&k2, user_q2, sizeof(k2))) return -EFAULT;

    int ret = nymya_3324_zz_interaction(&k1, &k2, theta);
    if (ret) return ret;

    if (copy_to_user(user_q1, &k1, sizeof(k1))) return -EFAULT;
    if (copy_to_user(user_q2, &k2, sizeof(k2))) return -EFAULT;
    return 0;
}
#endif // __KERNEL__

