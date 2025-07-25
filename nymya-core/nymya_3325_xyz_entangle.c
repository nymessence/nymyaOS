#include "nymya.h" // Include the header file for definitions

// Note: The actual implementations for fixed_cos, fixed_sin, and log_symbolic_event
// are assumed to be provided elsewhere, as they are declared in nymya.h but not defined here.

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_nymya_3325_xyz_entangle NYMYA_XYZ_CODE

/**
 * nymya_3325_xyz_entangle - Applies an XX+YY+ZZ entanglement operation to two qubits (userland version).
 *
 * This function performs a full XX+YY+ZZ type entanglement operation between two qubits
 * in user space. It applies a rotation to the amplitudes of both qubits using a complex
 * number derived from the given angle 'theta'. The first qubit's amplitude is multiplied
 * by the rotation complex number, and the second qubit's amplitude is multiplied by the
 * conjugate of the rotation complex number.
 *
 * The function logs the event symbolically for traceability.
 *
 * @q1: Pointer to the first qubit (nymya_qubit struct).
 * @q2: Pointer to the second qubit (nymya_qubit struct).
 * @theta: Entanglement angle in radians.
 *
 * Returns:
 * - 0 on success.
 * - -1 if either qubit pointer is NULL (invalid input).
 */
int nymya_3325_xyz_entangle(nymya_qubit* q1, nymya_qubit* q2, double theta) {
    // Basic null pointer check
    if (!q1 || !q2) return -1;

    // In userspace, we use native double precision complex numbers and math functions.
    // The fixed_cos and fixed_sin are kernel-specific.
    // We'll use standard C complex math for userspace.
    // Note: The nymya.h defines make_complex, complex_mul, complex_conj for userspace
    // using _Complex double and standard math functions.

    // Construct the rotation complex number e^(i*theta)
    complex_double rot = complex_exp_i(theta); // Uses cexp(I * theta) in userspace

    // Apply the rotation to the qubit amplitudes
    q1->amplitude = complex_mul(q1->amplitude, rot);
    q2->amplitude = complex_mul(q2->amplitude, complex_conj(rot));

    // Log the symbolic event (assumed to be implemented for userspace as well)
    log_symbolic_event("XYZ", q1->id, q1->tag, "Full XX+YY+ZZ entanglement");
    return 0;
}

#else // __KERNEL__

// Kernel-specific includes, only compiled when __KERNEL__ is defined
#include <linux/uaccess.h> // Required for copy_from_user, copy_to_user
#include <linux/syscalls.h> // Required for SYSCALL_DEFINE macros
#include <linux/printk.h>   // Required for pr_err

/**
 * nymya_3325_xyz_entangle - Applies an XX+YY+ZZ entanglement operation to two qubits (kernel version).
 *
 * This system call performs a full XX+YY+ZZ type entanglement operation between two qubits.
 * It copies the qubit structures from user space to kernel space, performs fixed-point
 * trigonometric calculations to construct a rotation complex number, applies this rotation
 * to the amplitudes of both qubits (multiplying the first by the rotation and the second
 * by its conjugate), and then copies the modified qubits back to user space.
 *
 * Error handling includes checks for null user pointers and failures during data copy
 * between user and kernel space. The operation is logged symbolically for debugging
 * and traceability.
 *
 * @user_q1: Pointer to the first qubit in user space (nymya_qubit struct).
 * @user_q2: Pointer to the second qubit in user space (nymya_qubit struct).
 * @fixed_theta: Entanglement angle in Q32.32 fixed-point format.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if either user_q1 or user_q2 is NULL.
 * - -EFAULT if copying data between user and kernel space fails.
 */
SYSCALL_DEFINE3(
    nymya_3325_xyz_entangle,
    struct nymya_qubit __user *, user_q1, // Pointer to user-space qubit 1
    struct nymya_qubit __user *, user_q2, // Pointer to user-space qubit 2
    int64_t, fixed_theta                  // Angle for entanglement, now fixed-point
) {
    struct nymya_qubit k_q1, k_q2; // Kernel-space copies of qubits
    int ret = 0; // Return value for syscall

    // 1. Check for null pointers from user-space
    if (!user_q1 || !user_q2) {
        pr_err("nymya_3325_xyz_entangle: Null user qubit pointer(s)\n");
        return -EINVAL; // Invalid argument
    }

    // 2. Copy the qubit structures from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3325_xyz_entangle: Failed to copy k_q1 from user\n");
        return -EFAULT; // Bad address
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3325_xyz_entangle: Failed to copy k_q2 from user\n");
        return -EFAULT; // Bad address
    }

    // 3. Perform fixed-point trigonometric calculations and complex number construction
    // fixed_theta is already in fixed-point format, so no conversion from double is needed here.
    int64_t fixed_cos_val = fixed_cos(fixed_theta);
    int64_t fixed_sin_val = fixed_sin(fixed_theta);

    // Construct the rotation complex number (fixed-point representation)
    // using the make_complex function defined in nymya.h.
    // This now directly takes fixed-point int64_t values, removing floating-point usage.
    complex_double rot = make_complex(fixed_cos_val, fixed_sin_val);

    // 4. Apply the rotation to the qubit amplitudes
    k_q1.amplitude = complex_mul(k_q1.amplitude, rot);
    k_q2.amplitude = complex_mul(k_q2.amplitude, complex_conj(rot));

    // 5. Log the symbolic event for the entanglement
    // Assumes log_symbolic_event is available in kernel context
    log_symbolic_event("XYZ", k_q1.id, k_q1.tag, "Full XX+YY+ZZ entanglement");

    // 6. Copy the modified qubits back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3325_xyz_entangle: Failed to copy k_q1 to user\n");
        ret = -EFAULT; // Bad address
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3325_xyz_entangle: Failed to copy k_q2 to user\n");
        ret = -EFAULT; // Bad address
    }

    return ret; // Return 0 on success, or error code if any copy_to_user failed
}

#endif

