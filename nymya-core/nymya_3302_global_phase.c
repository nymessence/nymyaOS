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
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/module.h> // Required for EXPORT_SYMBOL_GPL
    #include <linux/string.h> // For snprintf
#endif

// These macros are for userland only, where complex.h is available.
// In kernel, fixed-point complex operations are used directly.
#ifndef __KERNEL__
#define nymya_cexp(theta) cexp(I * (theta))
#define nymya_cmul(a, b)  ((a) * (b))
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

    // In userland, we directly apply the complex multiplication.
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

    // Calculate the complex phase factor e^(i*theta_fixed) using fixed-point trig functions
    phase_factor.re = fixed_cos(theta_fixed);
    phase_factor.im = fixed_sin(theta_fixed);

    // Apply the phase shift by complex multiplication
    q->amplitude = complex_mul(q->amplitude, phase_factor);

    // log_symbolic_event will be called in the syscall wrapper with formatted message
    return 0;
}
// Export the symbol for this function so other kernel modules/code can call it directly.
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
    int64_t, theta_fixed) { // CHANGED: Directly take fixed-point theta

    struct nymya_qubit kq;
    int ret;
    char log_msg[128]; // For formatted log message

    if (!user_q)
        return -EINVAL;

    // Copy qubit from user space
    if (copy_from_user(&kq, user_q, sizeof(kq)))
        return -EFAULT;

    // No need to copy theta_double or convert, as theta_fixed is passed directly.

    // Call the core logic function
    ret = nymya_3302_global_phase(&kq, theta_fixed);

    if (ret) // Propagate error from core function
        return ret;

    // Format log message with the fixed-point value.
    snprintf(log_msg, sizeof(log_msg), "Applied phase shift (fixed-point)=%lld", (long long)theta_fixed);
    log_symbolic_event("GPHASE", kq.id, kq.tag, log_msg);

    // Copy modified qubit back to user space
    if (copy_to_user(user_q, &kq, sizeof(kq)))
        return -EFAULT;

    return 0;
}

#endif

