// src/nymya_3322_xx_interaction.c

#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
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

#else

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

    // Validate the user-space pointers
    if (!user_q1 || !user_q2)
        return -EINVAL;  // Return error if any of the pointers are NULL

    // Copy the qubit structs from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1)))
        return -EFAULT;  // Return error if memory copy fails

    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2)))
        return -EFAULT;  // Return error if memory copy fails

    // Calculate the fixed-point cos and sin values for the phase
    int64_t cos_val = fixed_cos(theta);  // Real part of the phase
    int64_t sin_val = fixed_sin(theta);  // Imaginary part of the phase

    // Calculate the real and imaginary components for the phase
    int64_t real_phase = cos_val;  // Real part
    int64_t imag_phase = sin_val;  // Imaginary part

    // Apply the XX interaction phase to the first qubit's amplitude
    int64_t new_re1 = k_q1.amplitude.re * real_phase - k_q1.amplitude.im * imag_phase;
    int64_t new_im1 = k_q1.amplitude.re * imag_phase + k_q1.amplitude.im * real_phase;

    // Apply the XX interaction phase to the second qubit's amplitude
    int64_t new_re2 = k_q2.amplitude.re * real_phase - k_q2.amplitude.im * imag_phase;
    int64_t new_im2 = k_q2.amplitude.re * imag_phase + k_q2.amplitude.im * real_phase;

    // Update the amplitudes for both qubits
    k_q1.amplitude.re = new_re1;
    k_q1.amplitude.im = new_im1;

    k_q2.amplitude.re = new_re2;
    k_q2.amplitude.im = new_im2;

    // Log the symbolic event indicating the XX interaction has been applied
    log_symbolic_event("XX", k_q1.id, k_q1.tag, "Applied XX interaction with partner");

    // Copy the modified qubits back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;  // Return error if memory copy fails
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;  // Return error if memory copy fails

    return 0;  // Return success
}

#endif

