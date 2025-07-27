#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_nymya_3302_global_phase NYMYA_GLOBAL_PHASE_CODE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

// Safely override complex math macros only in userland simulation
#ifdef nymya_cexp
#undef nymya_cexp
#endif
#define nymya_cexp(theta) cexp(I * (theta))

#ifdef nymya_cmul
#undef nymya_cmul
#endif
#define nymya_cmul(a, b) ((a) * (b))

#else
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/module.h>  // Required for EXPORT_SYMBOL_GPL
#include <linux/string.h>  // For snprintf
#endif

/**
 * nymya_3302_global_phase - Apply global phase shift to a symbolic qubit
 * @q: Pointer to a symbolic qubit
 * @theta: Phase angle in radians (double for userland).
 *
 * Multiplies the qubit’s amplitude by e^(iθ), applying a global phase.
 * No probability amplitudes are altered, only the phase.
 * Logs symbolic event with phase shift details.
 *
 * Returns 0 on success, or -1 if the input is invalid.
 */
#ifndef __KERNEL__

int nymya_3302_global_phase(nymya_qubit* q, double theta) {
    if (!q) return -1;

    complex_double phase = nymya_cexp(theta);
    q->amplitude = nymya_cmul(q->amplitude, phase);

    char log_msg[128];
    snprintf(log_msg, sizeof(log_msg), "Applied phase shift θ=%.3f rad", theta);
    log_symbolic_event("GPHASE", q->id, q->tag, log_msg);

    return 0;
}

#else // __KERNEL__

/**
 * nymya_3302_global_phase - Core kernel function to apply global phase shift.
 * @q: Pointer to the kernel-space qubit structure.
 * @theta_fixed: Phase angle in fixed-point (int64_t) format.
 *
 * Multiplies the qubit's amplitude by e^(i*theta_fixed), applying a global phase.
 * This function is designed to be called directly by other kernel code.
 *
 * Returns 0 on success, -EINVAL if the qubit pointer is NULL.
 */
int nymya_3302_global_phase(struct nymya_qubit *q, int64_t theta_fixed) {
    complex_double phase_factor;

    if (!q) {
        pr_err("NYMYA: nymya_3302_global_phase received NULL qubit pointer\n");
        return -EINVAL;
    }

    phase_factor.re = fixed_cos(theta_fixed);
    phase_factor.im = fixed_sin(theta_fixed);

    q->amplitude = complex_mul(q->amplitude, phase_factor);

    return 0;
}

EXPORT_SYMBOL_GPL(nymya_3302_global_phase);

/**
 * SYSCALL_DEFINE2(nymya_3302_global_phase) - Kernel system call for global phase shift.
 * @user_q: User-space pointer to the qubit struct.
 * @theta_fixed: Phase angle already converted to fixed-point (int64_t) from user space.
 *
 * This syscall handles copying the qubit from user space, calls the core global phase
 * function, and copies the modified qubit back to user space. The phase angle
 * is expected to be provided as a fixed-point integer directly by userland.
 *
 * Returns 0 on success, -EINVAL on invalid pointers, -EFAULT on copy errors.
 */
SYSCALL_DEFINE2(nymya_3302_global_phase,
    struct nymya_qubit __user *, user_q,
    int64_t, theta_fixed)
{
    struct nymya_qubit kq;
    int ret;
    char log_msg[128];

    if (!user_q)
        return -EINVAL;

    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    ret = nymya_3302_global_phase(&kq, theta_fixed);
    if (ret)
        return ret;

    snprintf(log_msg, sizeof(log_msg), "Applied phase shift (fixed-point)=%lld", (long long)theta_fixed);
    log_symbolic_event("GPHASE", kq.id, kq.tag, log_msg);

    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif

