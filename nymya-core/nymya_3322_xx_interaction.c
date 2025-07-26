// src/nymya_3322_xx_interaction.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_nymya_3322_xx_interaction NYMYA_XX_CODE

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/module.h> // Required for EXPORT_SYMBOL_GPL
#endif

/**
 * nymya_3322_xx_interaction - Applies an XX interaction between two qubits.
 * 
 * This function applies a quantum interaction between two qubits in a way
 * that rotates both qubits' amplitudes according to a phase determined by
 * the angle `theta`. The operation is typically used in quantum circuits
 * that implement entangling operations, such as controlled operations.
 * 
 * The phase applied to the qubits is e^(i * theta), which involves both a 
 * real and imaginary component. In kernel space, the complex arithmetic
 * is handled manually with fixed-point calculations to avoid the use of
 * floating-point operations (not supported in the kernel).
 * 
 * The function logs the interaction for traceability in a symbolic event log.
 * 
 * @q1: Pointer to the first qubit to be rotated (nymya_qubit struct).
 * @q2: Pointer to the second qubit to be rotated (nymya_qubit struct).
 * @theta: Rotation angle in radians (double precision).
 * 
 * Returns:
 * - 0 on success (the interaction was applied successfully).
 * - -1 if either qubit pointer is NULL (invalid input).
 */
#ifndef __KERNEL__

int nymya_3322_xx_interaction(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    if (!q1 || !q2) return -1;  // Check for null pointers

    // Apply the XX interaction phase e^(i * theta), using complex arithmetic
    // We use _Complex double for phase calculation, and multiply qubit amplitudes by this phase
    _Complex double phase = cos(theta) + I * sin(theta);
    q1->amplitude *= phase;  // Apply phase to first qubit
    q2->amplitude *= phase;  // Apply phase to second qubit

    // Log the symbolic event indicating the XX interaction has been applied
    log_symbolic_event("XX", q1->id, q1->tag, "Applied XX interaction with partner");
    return 0;  // Return success
}

#else // __KERNEL__

/**
 * nymya_3322_xx_interaction - Core kernel function to apply an XX interaction.
 * @kq1: Pointer to the first qubit struct in kernel space.
 * @kq2: Pointer to the second qubit struct in kernel space.
 * @theta: Rotation angle in radians (fixed-point representation).
 *
 * This function implements the core logic for the XX interaction between two qubits.
 * It calculates the e^(i * theta) phase using fixed-point trigonometric functions
 * and applies this phase to the complex amplitudes of both qubits. The amplitudes
 * are updated directly within the provided kernel-space qubit structs.
 * A symbolic event is logged to record the interaction.
 *
 * Returns:
 * - 0 on success.
 * - Negative error code on failure (currently always returns 0 for valid inputs).
 */
int nymya_3322_xx_interaction(struct nymya_qubit *kq1, struct nymya_qubit *kq2, int64_t theta) {
    // Calculate the fixed-point cos and sin values for the phase
    int64_t cos_val = fixed_cos(theta);  // Real part of the phase
    int64_t sin_val = fixed_sin(theta);  // Imaginary part of the phase

    // Calculate the real and imaginary components for the phase
    int64_t real_phase = cos_val;  // Real part
    int64_t imag_phase = sin_val;  // Imaginary part

    // Apply the XX interaction phase to the first qubit's amplitude
    // (A + Bi) * (C + Di) = (AC - BD) + (AD + BC)i
    int64_t new_re1 = (kq1->amplitude.re * real_phase - kq1->amplitude.im * imag_phase);
    int64_t new_im1 = (kq1->amplitude.re * imag_phase + kq1->amplitude.im * real_phase);

    // Apply the XX interaction phase to the second qubit's amplitude
    int64_t new_re2 = (kq2->amplitude.re * real_phase - kq2->amplitude.im * imag_phase);
    int64_t new_im2 = (kq2->amplitude.re * imag_phase + kq2->amplitude.im * real_phase);

    // Update the amplitudes for both qubits
    kq1->amplitude.re = new_re1;
    kq1->amplitude.im = new_im1;

    kq2->amplitude.re = new_re2;
    kq2->amplitude.im = new_im2;

    // Log the symbolic event indicating the XX interaction has been applied
    log_symbolic_event("XX", kq1->id, kq1->tag, "Applied XX interaction with partner");

    return 0; // Success
}
EXPORT_SYMBOL_GPL(nymya_3322_xx_interaction);


/**
 * SYSCALL_DEFINE3(nymya_3322_xx_interaction) - Kernel syscall for XX interaction between two qubits.
 * 
 * This syscall implements the XX interaction for two qubits in the kernel space.
 * The interaction applies a quantum phase to both qubits, modifying their amplitudes.
 * The phase applied is e^(i * theta), where `theta` is the rotation angle passed to the syscall.
 * 
 * The amplitude of each qubit is updated by multiplying with this phase. Since kernel space does not 
 * support floating-point arithmetic or complex numbers, we manually handle the real and imaginary parts.
 * The amplitude is updated using fixed-point calculations.
 * 
 * The modified qubits are then copied back to user space after the operation.
 * 
 * @user_q1: User-space pointer to the first qubit struct.
 * @user_q2: User-space pointer to the second qubit struct.
 * @theta: Rotation angle in radians (fixed-point representation).
 * 
 * Returns:
 * - 0 on success.
 * - -EINVAL if either user_q1 or user_q2 is NULL (invalid input).
 * - -EFAULT if copying data to/from user space fails (memory access issues).
 */
SYSCALL_DEFINE3(nymya_3322_xx_interaction,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    int64_t, theta) {

    struct nymya_qubit k_q1, k_q2;  // Kernel space qubits
    int ret;

    // Validate the user-space pointers
    if (!user_q1 || !user_q2)
        return -EINVAL;  // Return error if any of the pointers are NULL

    // Copy the qubit structs from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;  // Return error if memory copy fails

    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;  // Return error if memory copy fails

    // Call the newly created core function with kernel-space variables
    ret = nymya_3322_xx_interaction(&k_q1, &k_q2, theta);
    if (ret)
        return ret; // Propagate error from core function, if any

    // Copy the modified qubits back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;  // Return error if memory copy fails
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;  // Return error if memory copy fails

    return 0;  // Return success
}

#endif
