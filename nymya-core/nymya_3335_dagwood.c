// src/nymya_3335_dagwood.c

#include "nymya.h" // Common definitions like complex_double

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>    // Userland only
#define __NR_nymya_3335_dagwood NYMYA_DAGWOOD_CODE

    #include <stdlib.h>  // Userland only
    #include <math.h>    // For cabs in userland
    #include <complex.h> // For _Complex double in userland
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h> // For -EINVAL, -EFAULT
    #include <linux/module.h> // Required for EXPORT_SYMBOL_GPL
    // No math.h or complex.h in kernel; fixed-point math assumed for amplitude operations
#endif

#ifndef __KERNEL__

/**
 * nymya_3335_dagwood - Applies a Dagwood gate to three qubits (userland).
 * @q1: Pointer to the first control qubit.
 * @q2: Pointer to the first target qubit.
 * @q3: Pointer to the second target qubit.
 *
 * This function implements a Dagwood gate. If the control qubit's amplitude
 * magnitude squared is above a threshold (representing a '1' state), it swaps
 * the amplitudes of the two target qubits (q2 and q3). Otherwise, no action is taken.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3335_dagwood(nymya_qubit* q1, nymya_qubit* q2, nymya_qubit* q3) {
    if (!q1 || !q2 || !q3) return -1;

    // In userland, q1->amplitude is _Complex double.
    // Calculate the magnitude squared directly using cabs().
    double ctrl_mag_sq = cabs(q1->amplitude) * cabs(q1->amplitude);

    // Define the threshold for control qubit being '1' (0.5^2 = 0.25)
    const double threshold_sq = 0.25;

    // If the control qubit's amplitude magnitude squared is above the threshold, perform SWAP
    if (ctrl_mag_sq > threshold_sq) {
        nymya_3313_swap(q2, q3); // Call userland SWAP gate
        log_symbolic_event("DAGWOOD", q1->id, q1->tag, "Dagwood swap applied");
    } else {
        log_symbolic_event("DAGWOOD", q1->id, q1->tag, "Control=0, no swap");
    }
    return 0;
}

#else // __KERNEL__

/**
 * fixed_point_magnitude_sq - Calculates the squared magnitude of a fixed-point complex number.
 * @c: The fixed-point complex number.
 *
 * Returns:
 * The squared magnitude in fixed-point format.
 */
static inline int64_t fixed_point_magnitude_sq(complex_double c) {
    // Magnitude squared = re*re + im*im
    // Note: This assumes fixed_point_mul is available via nymya.h.
    return fixed_point_mul(c.re, c.re) + fixed_point_mul(c.im, c.im);
}

/**
 * nymya_3335_dagwood - Applies a Dagwood gate to three qubits (kernel-side core logic).
 * @k_q1: Pointer to the kernel-space copy of the control qubit.
 * @k_q2: Pointer to the kernel-space copy of the first target qubit.
 * @k_q3: Pointer to the kernel-space copy of the second target qubit.
 *
 * This function implements the core Dagwood gate logic in kernel space.
 * If the control qubit's fixed-point amplitude magnitude squared is above
 * a predefined threshold (representing a '1' state), it performs a swap
 * between the amplitudes of the two target qubits (k_q2 and k_q3) using
 * the kernel's nymya_3313_swap function. Otherwise, no action is taken.
 *
 * This function operates on kernel-space qubit structures directly.
 *
 * Returns:
 * - 0 on success.
 * - Error code from underlying gate operations (e.g., from nymya_3313_swap),
 *   if the swap was attempted and failed.
 */
int nymya_3335_dagwood(struct nymya_qubit *k_q1, struct nymya_qubit *k_q2, struct nymya_qubit *k_q3) {
    int ret = 0; // Initialize to success (no operation or successful operation)

    // Threshold for control qubit being '1' (0.5^2 = 0.25 in floating point)
    // Convert 0.25 to fixed-point squared value
    const int64_t threshold_sq_fp = FIXED_POINT_SCALE / 4; // Represents 0.25 in Q32.32

    if (fixed_point_magnitude_sq(k_q1->amplitude) > threshold_sq_fp) {
        // Control qubit is "on" (predominantly |1>)
        // Call the kernel version of nymya_3313_swap
        ret = nymya_3313_swap(k_q2, k_q3);
        if (ret == 0) { // Only log success or failure of SWAP, not gate itself
            log_symbolic_event("DAGWOOD", k_q1->id, k_q1->tag, "Dagwood swap applied");
        }
        // If ret != 0, it means nymya_3313_swap failed, and we propagate that error.
    } else {
        // Control qubit is "off" (predominantly |0>)
        log_symbolic_event("DAGWOOD", k_q1->id, k_q1->tag, "Control=0, no swap");
    }

    return ret;
}
EXPORT_SYMBOL_GPL(nymya_3335_dagwood);



/**
 * SYSCALL_DEFINE3(nymya_3335_dagwood) - Kernel syscall for Dagwood gate.
 * @user_q1: User-space pointer to the control qubit.
 * @user_q2: User-space pointer to the first target qubit.
 * @user_q3: User-space pointer to the second target qubit.
 *
 * This syscall copies qubit data from user space, applies the Dagwood gate
 * logic using kernel-space functions, and then copies the modified data back.
 * The Dagwood gate conditionally swaps two target qubits based on a control qubit's state.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user pointer is invalid.
 * - -EFAULT if copying data to/from user space fails.
 * - Error code from underlying gate operations (e.g., nymya_3313_swap),
 *   if the core logic was executed and failed.
 */
SYSCALL_DEFINE3(nymya_3335_dagwood,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2,
    struct nymya_qubit __user *, user_q3) {

    struct nymya_qubit k_q1, k_q2, k_q3; // Kernel-space copies
    int ret = 0; // Initialize return value for syscall

    // 1. Validate user pointers
    if (!user_q1 || !user_q2 || !user_q3) {
        pr_err("nymya_3335_dagwood: Invalid user qubit pointers\n");
        return -EINVAL;
    }

    // 2. Copy qubit data from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3335_dagwood: Failed to copy k_q1 from user space\n");
        return -EFAULT;
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3335_dagwood: Failed to copy k_q2 from user space\n");
        return -EFAULT;
    }
    if (copy_from_user(&k_q3, user_q3, sizeof(k_q3))) {
        pr_err("nymya_3335_dagwood: Failed to copy k_q3 from user space\n");
        return -EFAULT;
    }

    // 3. Call the newly created core function
    ret = nymya_3335_dagwood(&k_q1, &k_q2, &k_q3);

    // Check if the core logic (specifically, the swap if performed) returned an error
    if (ret != 0) {
        pr_err("nymya_3335_dagwood: Core logic (swap) failed with error %d\n", ret);
        return ret; // Propagate the error from the core function
    }

    // 4. Copy modified qubit data back to user space
    // Note: k_q1 (control qubit) is typically not modified by Fredkin/Dagwood,
    // so it doesn't strictly need to be copied back unless its state *could* change.
    // However, copying it back is safer if there's any ambiguity or future changes.
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3335_dagwood: Failed to copy k_q1 to user space\n");
        return -EFAULT;
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3335_dagwood: Failed to copy k_q2 to user space\n");
        return -EFAULT;
    }
    if (copy_to_user(user_q3, &k_q3, sizeof(k_q3))) {
        pr_err("nymya_3335_dagwood: Failed to copy k_q3 to user space\n");
        return -EFAULT;
    }

    return 0; // Success
}

#endif
