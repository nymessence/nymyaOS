// src/nymya_3360_e5_projected_lattice.c

#include <stdlib.h>
// #include <math.h> // math.h is for floating-point, avoid in kernel
#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
    #include <math.h> // Include math.h for userspace for sqrt and pow
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/slab.h>
    // No math.h for kernel, fixed-point math assumed or implemented
#endif

#ifndef __KERNEL__

/**
 * dist5d - Calculates the Euclidean distance between two nymya_qpos5d points (userland version).
 * @a: The first 5D qubit position.
 * @b: The second 5D qubit position.
 *
 * This function computes the standard Euclidean distance between two points
 * in 5D space using floating-point arithmetic.
 *
 * Returns:
 * The distance as a double.
 */
double dist5d(nymya_qpos5d a, nymya_qpos5d b) {
    return sqrt(pow(a.x - b.x, 2) +
                pow(a.y - b.y, 2) +
                pow(a.z - b.z, 2) +
                pow(a.w - b.w, 2) +
                pow(a.v - b.v, 2));
}

/**
 * nymya_3360_e5_projected_lattice - Applies quantum operations on qubits in an E5 projected lattice (userland version).
 * @q: An array of nymya_qpos5d structures, representing qubits with their 5D positions.
 * @count: The number of qubits in the array.
 *
 * This function simulates quantum operations on qubits arranged in a 5D projection
 * of an E5 lattice structure. It initializes each qubit with a Hadamard gate
 * and then applies CNOT gates between qubits that are within a certain interaction
 * distance (epsilon).
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubits array is NULL or count is less than 40 (minimum for a meaningful E5 projection).
 */
int nymya_3360_e5_projected_lattice(nymya_qpos5d q[], size_t count) {
    // Check for valid input array and minimum count for E5 projected lattice
    if (!q || count < 40) return -1;
    const double epsilon = 1.05;

    // Apply Hadamard gate to each qubit to put them in superposition
    for (size_t i = 0; i < count; i++) {
        // q[i].q is a nymya_qubit struct, pass its address to hadamard
        hadamard(&q[i].q);
    }

    // Apply CNOT gates between neighboring qubits based on distance
    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            // Calculate distance between qubit positions
            if (dist5d(q[i], q[j]) <= epsilon) {
                // Apply CNOT gate, passing addresses of the qubit structs
                cnot(&q[i].q, &q[j].q);
            }
        }
    }

    // Log the symbolic event for the E5 projected lattice entanglement
    // Access struct members directly using '.' operator
    log_symbolic_event("E5_PROJECTED", q[0].q.id, q[0].q.tag,
        "Projected E5 root lattice entanglement");
    return 0;
}

#else // __KERNEL__

/**
 * fixed_point_square - Calculates the square of a fixed-point number.
 * @val: The fixed-point number.
 *
 * Returns:
 * The square of the fixed-point number, adjusted for the fixed-point scale.
 */
static inline int64_t fixed_point_square(int64_t val) {
    // Perform multiplication using __int128 to prevent overflow, then shift.
    return (int64_t)((__int128)val * val >> 32);
}

/**
 * dist5d_squared_k - Calculates the squared Euclidean distance between two nymya_qpos5d points (kernel version).
 * @a: Pointer to the first 5D qubit position.
 * @b: Pointer to the second 5D qubit position.
 *
 * This function computes the squared Euclidean distance between two points
 * in 5D space using fixed-point arithmetic. It avoids floating-point operations
 * which are generally disallowed in kernel code.
 *
 * Returns:
 * The squared distance as an int64_t (fixed-point).
 */
static int64_t dist5d_squared_k(const nymya_qpos5d *a, const nymya_qpos5d *b) {
    // Calculate differences in fixed-point
    int64_t dx = (int64_t)((a->x - b->x) * FIXED_POINT_SCALE);
    int64_t dy = (int64_t)((a->y - b->y) * FIXED_POINT_SCALE);
    int64_t dz = (int64_t)((a->z - b->z) * FIXED_POINT_SCALE);
    int64_t dw = (int64_t)((a->w - b->w) * FIXED_POINT_SCALE);
    int64_t dv = (int64_t)((a->v - b->v) * FIXED_POINT_SCALE);

    // Calculate squared differences in fixed-point
    int64_t dx_sq = fixed_point_square(dx);
    int64_t dy_sq = fixed_point_square(dy);
    int64_t dz_sq = fixed_point_square(dz);
    int64_t dw_sq = fixed_point_square(dw);
    int64_t dv_sq = fixed_point_square(dv);

    // Sum the squared differences. No shift needed here as fixed_point_square already handles it.
    return dx_sq + dy_sq + dz_sq + dw_sq + dv_sq;
}

/**
 * nymya_3360_e5_projected_lattice - Applies quantum operations on qubits in an E5 projected lattice (kernel version).
 * @user_q: Pointer to an array of nymya_qpos5d structures in user space.
 * @count: The number of qubits in the array.
 *
 * This system call simulates quantum operations on qubits arranged in a 5D projection
 * of an E5 lattice structure. It copies qubit data from user space to kernel space,
 * applies Hadamard gates to each qubit, and then applies CNOT gates between qubits
 * that are within a certain interaction distance (using fixed-point squared distance).
 * Finally, it copies the modified qubit data back to user space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if user_q is NULL or count is less than 40.
 * - -ENOMEM if kernel memory allocation fails.
 * - -EFAULT if copying data between user and kernel space fails.
 */
SYSCALL_DEFINE2(nymya_3360_e5_projected_lattice,
    struct nymya_qpos5d __user *, user_q,
    size_t, count) {

    int ret = 0; // Return value for syscall
    nymya_qpos5d *k_q = NULL; // Kernel-space copy of qubits

    // 1. Validate input arguments
    if (!user_q || count < 40) {
        pr_err("nymya_3360_e5_projected_lattice: Invalid user_q pointer or count (%zu < 40)\n", count);
        return -EINVAL;
    }

    // 2. Allocate kernel memory for qubit data
    k_q = kmalloc_array(count, sizeof(nymya_qpos5d), GFP_KERNEL);
    if (!k_q) {
        pr_err("nymya_3360_e5_projected_lattice: Failed to allocate kernel memory for qubits\n");
        return -ENOMEM;
    }

    // 3. Copy qubit data from user space to kernel space
    if (copy_from_user(k_q, user_q, count * sizeof(nymya_qpos5d))) {
        pr_err("nymya_3360_e5_projected_lattice: Failed to copy qubits from user space\n");
        ret = -EFAULT;
        goto free_k_q; // Jump to cleanup
    }

    // Define the squared epsilon for fixed-point comparison
    // (1.05)^2 = 1.1025. Convert this to fixed-point.
    const int64_t fixed_epsilon_squared = (int64_t)(1.1025 * FIXED_POINT_SCALE);

    // 4. Apply Hadamard gate to each qubit
    for (size_t i = 0; i < count; i++) {
        // Pass the address of the nymya_qubit struct to the hadamard macro/function
        hadamard(&k_q[i].q);
    }

    // 5. Apply CNOT gates between neighboring qubits based on fixed-point squared distance
    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            // Calculate squared distance in fixed-point
            if (dist5d_squared_k(&k_q[i], &k_q[j]) <= fixed_epsilon_squared) {
                // Apply CNOT gate, passing addresses of the qubit structs
                cnot(&k_q[i].q, &k_q[j].q);
            }
        }
    }

    // 6. Log the symbolic event for the E5 projected lattice entanglement
    // Access struct members directly using '.' operator
    log_symbolic_event("E5_PROJECTED", k_q[0].q.id, k_q[0].q.tag,
        "Projected E5 root lattice entanglement");

    // 7. Copy the modified qubits back to user space
    if (copy_to_user(user_q, k_q, count * sizeof(nymya_qpos5d))) {
        pr_err("nymya_3360_e5_projected_lattice: Failed to copy qubits to user space\n");
        ret = -EFAULT;
    }

free_k_q:
    kfree(k_q); // Free allocated kernel memory
    return ret;
}

#endif

