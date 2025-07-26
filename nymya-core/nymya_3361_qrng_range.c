// src/nymya_3361_qrng_range.c

#include "nymya.h" // Defines: nymya_qubit, FIXED_POINT_SCALE, NYMYA_QRNG_CODE, make_complex, hadamard, global_phase, log_symbolic_event

#ifndef __KERNEL__
#include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h> // For malloc, free, srand, rand in userland
    #include <time.h>   // For time(NULL) in userland
    #include <math.h>   // Not strictly needed for this file, but often grouped with other userland includes
    #include <unistd.h> // For syscall
    #include <sys/syscall.h> // For syscall() function prototype
    #include <errno.h>  // For ENOMEM and other errno values in userland
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/slab.h> // For kmalloc, kfree
    #include <linux/random.h> // For get_random_u32
    #include <linux/module.h> // Required for EXPORT_SYMBOL_GPL
    // No stdlib.h, time.h, math.h for kernel
#endif

#ifndef __KERNEL__

// Define the syscall number for userland, using the code from nymya.h.
// This is necessary because syscall() expects the __NR_ prefix.
#define __NR_nymya_3361_qrng_range NYMYA_QRNG_CODE

/**
 * nymya_3361_qrng_range - Userland wrapper for the QRNG syscall.
 * @out: Pointer to an array where the generated random numbers will be stored.
 * @min: The minimum value for the random numbers (inclusive).
 * @max: The maximum value for the random numbers (inclusive).
 * @count: The number of random numbers to generate.
 *
 * This function simulates quantum random number generation in userland.
 * It initializes qubits, applies quantum gates (symbolically), and then
 * generates a random bit to determine the output value within the specified range.
 *
 * Returns:
 * - 0 on success.
 * - -1 if input parameters are invalid.
 */
int nymya_3361_qrng_range(uint64_t* out, uint64_t min, uint64_t max, size_t count) {
    if (!out || min >= max || count == 0) return -1;

    // In userland, we can still use stdlib's rand for demonstration or testing
    // if a true quantum RNG is not available.
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL)); // Seed RNG in userland
        seeded = 1;
    }

    // The userland function now primarily acts as a wrapper to call the kernel syscall.
    // The actual QRNG logic resides in the kernel implementation.
    long ret = syscall(__NR_nymya_3361_qrng_range, (unsigned long)out, min, max, count);

    return (int)ret;
}

#else // __KERNEL__

// External kernel functions for qubit operations and logging.
// These are assumed to be defined elsewhere in the kernel and linked,
// and their declarations are now expected to be in nymya.h.
extern int hadamard(nymya_qubit *q);
extern int global_phase(nymya_qubit *q, int64_t theta_fp);
extern int log_symbolic_event(const char* gate, uint64_t id, const char* tag, const char* msg);


/**
 * @brief Performs the core logic for Quantum Random Number Generation within a range.
 *
 * This function simulates quantum random number generation. It performs
 * symbolic quantum operations (Hadamard, Global Phase) on a conceptual qubit
 * for each number generated, and then uses a kernel-safe pseudo-random number
 * generator to produce a bit, which is scaled to the desired range.
 *
 * It handles the allocation of temporary kernel memory, the generation loop,
 * and copying the results back to user space.
 *
 * @param user_out Pointer to a user-space array of uint64_t where the generated numbers will be stored.
 * @param min The minimum value for the random numbers (inclusive).
 * @param max The maximum value for the random numbers (inclusive).
 * @param count The number of random numbers to generate.
 *
 * @return 0 on success.
 * @return -EINVAL if input parameters are invalid.
 * @return -ENOMEM if kernel memory allocation fails.
 * @return -EFAULT if copying data to user space fails.
 */
int nymya_3361_qrng_range(uint64_t __user *user_out, uint64_t min, uint64_t max, size_t count) {
    int ret = 0;
    uint64_t* k_out = NULL;

    if (!user_out || min >= max || count == 0)
        return -EINVAL;

    k_out = kmalloc(sizeof(uint64_t) * count, GFP_KERNEL);
    if (!k_out)
        return -ENOMEM;

    // Use get_random_u32 for kernel-safe random number generation.
    // This function draws from the kernel's entropy pool and does not
    // require explicit seeding within this syscall.

    for (size_t i = 0; i < count; i++) {
        // Conceptual qubit for symbolic operations
        struct nymya_qubit q = {
            .id = i,
            .tag = "qrng",
            .amplitude = make_complex(FIXED_POINT_SCALE, 0) // Initialize to |0> state in fixed-point
        };

        // Symbolic quantum operations
        // These functions are assumed to modify the amplitude of 'q' symbolically
        // or perform other logging/tracking.
        hadamard(&q);          // Symbolic superposition
        global_phase(&q, 0);   // Symbolic identity phase (0 in fixed-point)

        // Generate a pseudo-random bit using kernel's get_random_u32
        // We only need a bit, so we mask the result.
        int bit = get_random_u32() & 1; // Get a single random bit (0 or 1)

        // Map the random bit to either min or max.
        // This simulates a binary outcome from a quantum measurement.
        k_out[i] = bit == 0 ? min : max;

        // Log the symbolic event for the generated bit
        log_symbolic_event("QRNG_BIT", q.id, q.tag, bit ? "1" : "0");
    }

    // Copy generated numbers to user space
    if (copy_to_user(user_out, k_out, sizeof(uint64_t) * count)) {
        ret = -EFAULT;
    }

    kfree(k_out);
    return ret;
}
EXPORT_SYMBOL_GPL(nymya_3361_qrng_range);



/**
 * nymya_3361_qrng_range - Kernel syscall for Quantum Random Number Generation within a range.
 * @user_out: Pointer to a user-space array of uint64_t where the generated numbers will be stored.
 * @min: The minimum value for the random numbers (inclusive).
 * @max: The maximum value for the random numbers (inclusive).
 * @count: The number of random numbers to generate.
 *
 * This system call is a wrapper around the core `nymya_3361_qrng_range` function,
 * providing the standard syscall entry point.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if input parameters are invalid.
 * - -ENOMEM if kernel memory allocation fails.
 * - -EFAULT if copying data to user space fails.
 */
SYSCALL_DEFINE4(nymya_3361_qrng_range,
    uint64_t __user *, user_out,
    uint64_t, min,
    uint64_t, max,
    size_t, count) {
    // The actual logic, including input validation, memory allocation/deallocation,
    // QRNG simulation, and copy_to_user, is now encapsulated in the
    // nymya_3361_qrng_range function.
    return nymya_3361_qrng_range(user_out, min, max, count);
}

#endif
