#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_nymya_3323_yy_interaction NYMYA_YY_CODE

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/module.h> // Required for EXPORT_SYMBOL_GPL

/**
 * nymya_3323_yy_interaction - Applies a YY interaction between two kernel-space qubits.
 *
 * This function encapsulates the core logic for applying a YY interaction.
 * It modifies the amplitudes of two provided kernel-space qubit structures
 * by applying a phase rotation. The phase for the first qubit is e^(i * theta)
 * and for the second qubit is its complex conjugate e^(-i * theta) to maintain symmetry.
 *
 * In kernel space, floating-point and complex number support is limited, so fixed-point math
 * is used for trigonometric calculations and complex number multiplication.
 * The amplitude of the qubits is updated manually with the real and imaginary parts
 * calculated separately using fixed-point arithmetic.
 *
 * @kq1: Pointer to the first kernel-space qubit structure (nymya_qubit).
 * @kq2: Pointer to the second kernel-space qubit structure (nymya_qubit).
 * @theta: Rotation angle in radians, represented as a fixed-point integer.
 *
 * Returns:
 * - 0 on success.
 * - This function assumes valid, non-NULL pointers are passed and does not perform
 *   validation of input pointers, as that is typically handled by the calling syscall wrapper.
 */
int nymya_3323_yy_interaction(struct nymya_qubit *kq1, struct nymya_qubit *kq2, int64_t theta) {
    // Use fixed-point math to compute the trigonometric values
    int64_t cos_val = fixed_cos(theta); // Real part of the phase
    int64_t sin_val = fixed_sin(theta); // Imaginary part of the phase

    // Calculate the real and imaginary components for the phase (e^(i*theta))
    int64_t real_phase = cos_val;
    int64_t imag_phase = sin_val;

    // Apply the YY interaction phase to the first qubit's amplitude
    // (A + Bi) * (C + Di) = (AC - BD) + i(AD + BC)
    int64_t new_re1 = kq1->amplitude.re * real_phase - kq1->amplitude.im * imag_phase;
    int64_t new_im1 = kq1->amplitude.re * imag_phase + kq1->amplitude.im * real_phase;

    // Apply the inverse phase to the second qubit's amplitude
    // This is equivalent to multiplying by conj(e^(i*theta)), i.e., (C - Di)
    // (A + Bi) * (C - Di) = (AC + BD) + i(BC - AD)
    int64_t new_re2 = kq2->amplitude.re * real_phase + kq2->amplitude.im * imag_phase;
    int64_t new_im2 = kq2->amplitude.im * real_phase - kq2->amplitude.re * imag_phase;

    // Update the amplitudes for both qubits
    kq1->amplitude.re = new_re1;
    kq1->amplitude.im = new_im1;

    kq2->amplitude.re = new_re2;
    kq2->amplitude.im = new_im2;

    // Log the symbolic event indicating the YY interaction has been applied
    log_symbolic_event("YY", kq2->id, kq2->tag, "Applied YY interaction");

    return 0; // Return success
}
EXPORT_SYMBOL_GPL(nymya_3323_yy_interaction);

#endif

/**
 * nymya_3323_yy_interaction - Applies a YY interaction between two qubits.
 * 
 * The YY interaction applies a phase rotation to two qubits in a quantum system.
 * The phase rotation is represented by a complex exponential e^(i * theta) for the first qubit
 * and its complex conjugate e^(-i * theta) for the second qubit to maintain symmetry.
 * The interaction is commonly used in quantum circuits for entanglement operations, such as in 
 * quantum gates that operate on two qubits simultaneously.
 * 
 * The function logs the interaction for traceability, and updates the amplitude of the qubits 
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

int nymya_3323_yy_interaction(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    if (!q1 || !q2) return -1;  // Check for null pointers

    // Apply the YY interaction phase e^(i * theta) for the first qubit
    _Complex double rot = cos(theta) + I * sin(theta);
    q1->amplitude *= rot;  // Apply phase to first qubit
    
    // Apply the inverse phase e^(-i * theta) for the second qubit
    q2->amplitude *= conj(rot);  // Conjugate to apply the inverse phase

    // Log the symbolic event indicating the YY interaction has been applied
    log_symbolic_event("YY", q2->id, q2->tag, "Applied YY interaction");
    return 0;  // Return success
}

#else

/**
 * SYSCALL_DEFINE3(nymya_3323_yy_interaction) - Kernel syscall for YY interaction between two qubits.
 * 
 * This syscall implements the YY interaction for two qubits in the kernel space.
 * It acts as a wrapper that copies data between user space and kernel space,
 * and then calls the core kernel function to perform the actual YY interaction logic.
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
SYSCALL_DEFINE3(nymya_3323_yy_interaction,
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

    // Call the core interaction logic
    ret = nymya_3323_yy_interaction(&k_q1, &k_q2, theta);
    if (ret) {
        // Propagate error if the core function indicates a failure,
        // though currently it is designed to always return 0.
        return ret;
    }

    // Copy the modified qubits back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1)))
        return -EFAULT;  // Return error if memory copy fails
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2)))
        return -EFAULT;  // Return error if memory copy fails

    return 0;  // Return success
}

#endif
