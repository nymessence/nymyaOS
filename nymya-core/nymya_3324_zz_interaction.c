// src/nymya_3324_zz_interaction.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_nymya_3324_zz_interaction NYMYA_ZZ_CODE

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
 * nymya_3324_zz_interaction - Applies a ZZ interaction between two qubits.
 * 
 * The ZZ interaction applies a phase rotation to both qubits in a quantum system, where the 
 * phase is represented by e^(i * theta). Both qubits are updated with the same phase, which is 
 * commonly used in quantum circuits to introduce a conditional interaction between qubits.
 * 
 * The function logs the interaction for traceability and updates the amplitudes of the qubits 
 * according to the applied phase.
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

int nymya_3324_zz_interaction(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    if (!q1 || !q2) return -1;  // Check for null pointers

    // Apply the ZZ interaction phase e^(i * theta) to both qubits
    _Complex double zphase = cexp(I * theta);  // Complex exponential for the phase
    q1->amplitude *= zphase;  // Apply phase to the first qubit
    q2->amplitude *= zphase;  // Apply phase to the second qubit

    // Log the symbolic event indicating the ZZ interaction has been applied
    log_symbolic_event("ZZ", q2->id, q2->tag, "Applied ZZ phase coupling");
    return 0;  // Return success
}

#else

/**
 * SYSCALL_DEFINE3(nymya_3324_zz_interaction) - Kernel syscall for ZZ interaction between two qubits.
 * 
 * This syscall implements the ZZ interaction for two qubits in the kernel space.
 * The interaction applies a phase to both qubits' amplitudes, represented by e^(i * theta).
 * In kernel space, we use fixed-point math for trigonometric functions to avoid floating-point operations.
 * The amplitude of both qubits is updated manually with the real and imaginary parts computed separately.
 * 
 * After applying the interaction, the modified qubits are copied back to user space.
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
SYSCALL_DEFINE3(nymya_3324_zz_interaction,
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

    // Use fixed-point math to compute the trigonometric values
    int64_t cos_val = fixed_cos(theta);  // Fixed-point cosine
    int64_t sin_val = fixed_sin(theta);  // Fixed-point sine

    // Compute the real and imaginary parts for the ZZ phase e^(i * theta)
    int64_t real_part = k_q1.amplitude.re * cos_val - k_q1.amplitude.im * sin_val;
    int64_t imag_part = k_q1.amplitude.re * sin_val + k_q1.amplitude.im * cos_val;

    // Apply the ZZ phase to the first qubit's amplitude
    k_q1.amplitude.re = real_part;
    k_q1.amplitude.im = imag_part;

    // Apply the same phase to the second qubit's amplitude (since it's a ZZ interaction)
    real_part = k_q2.amplitude.re * cos_val - k_q2.amplitude.im * sin_val;
    imag_part = k_q2.amplitude.re * sin_val + k_q2.amplitude.im * cos_val;

    // Update the second qubit's amplitude
    k_q2.amplitude.re = real_part;
    k_q2.amplitude.im = imag_part;

    // Log the symbolic event indicating the ZZ interaction has been applied
    log_symbolic_event("ZZ", k_q2.id, k_q2.tag, "Applied ZZ phase coupling");

    // Copy the modified qubits back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;  // Return error if memory copy fails
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;  // Return error if memory copy fails

    return 0;  // Return success
}

#endif

