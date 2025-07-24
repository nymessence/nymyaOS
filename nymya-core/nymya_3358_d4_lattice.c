// src/nymya_3358_d4_lattice.c

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
 * dist4d - Calculates the Euclidean distance between two nymya_qpos4d points (userland version).
 * @a: The first 4D qubit position.
 * @b: The second 4D qubit position.
 *
 * This function computes the standard Euclidean distance between two points
 * in 4D space using floating-point arithmetic.
 *
 * Returns:
 * The distance as a double.
 */
double dist4d(nymya_qpos4d a, nymya_qpos4d b) {
    return sqrt(pow(a.x - b.x, 2) +
                pow(a.y - b.y, 2) +
                pow(a.z - b.z, 2) +
                pow(a.w - b.w, 2));
}

/**
 * nymya_3358_d4_lattice - Applies quantum operations on qubits in a D4 lattice (userland version).
 * @q: An array of nymya_qpos4d structures, representing qubits with their 4D positions.
 * @count: The number of qubits in the array.
 *
 * This function simulates quantum operations on qubits arranged in a D4 lattice
 * structure in 4D space. It initializes each qubit with a Hadamard gate
 * and then applies CNOT gates between qubits that are within a certain interaction
 * distance (epsilon).
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubits array is NULL or count is less than 24 (minimum for a meaningful D4 unit).
 */
int nymya_3358_d4_lattice(nymya_qpos4d q[], size_t count) {
    // Check for valid input array and minimum count for D4 lattice
    if (!q || count < 24) return -1;

    // Epsilon defines the interaction radius for CNOT gates.
    // Qubits within this distance are considered "neighbors" for entanglement.
    const double epsilon = 1.01;

    // Apply Hadamard gate to each qubit to put them in superposition
    for (size_t i = 0; i < count; i++) {
        // q[i].q is a nymya_qubit struct, pass its address to hadamard
        hadamard(&q[i].q);
    }

    // Apply CNOT gates between neighboring qubits based on distance
    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            // Calculate distance between qubit positions
            if (dist4d(q[i], q[j]) <= epsilon) {
                // Apply CNOT gate, passing addresses of the qubit structs
                cnot(&q[i].q, &q[j].q);
            }
        }
    }

    // Log the symbolic event for the D4 lattice entanglement
    // Access struct members directly using '.' operator
    log_symbolic_event("D4_LATTICE", q[0].q.id, q[0].q.tag, "D4 lattice entangled in 4D");
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
 * dist4d_squared_k - Calculates the squared Euclidean distance between two nymya_qpos4d points (kernel version).
 * @a: Pointer to the first 4D qubit position.
 * @b: Pointer to the second 4D qubit position.
 *
 * This function computes the squared Euclidean distance between two points
 * in 4D space using fixed-point arithmetic. It avoids floating-point operations
 * which are generally disallowed in kernel code.
 *
 * Returns:
 * The squared distance as an int64_t (fixed-point).
 */
static int64_t dist4d_squared_k(const nymya_qpos4d *a, const nymya_qpos4d *b) {
    // Calculate differences in fixed-point
    int64_t dx = (int64_t)((a->x - b->x) * FIXED_POINT_SCALE);
    int64_t dy = (int64_t)((a->y - b->y) * FIXED_POINT_SCALE);
    int64_t dz = (int64_t)((a->z - b->z) * FIXED_POINT_SCALE);
    int64_t dw = (int64_t)((a->w - b->w) * FIXED_POINT_SCALE);

    // Calculate squared differences in fixed-point
    int64_t dx_sq = fixed_point_square(dx);
    int64_t dy_sq = fixed_point_square(dy);
    int64_t dz_sq = fixed_point_square(dz);
    int64_t dw_sq = fixed_point_square(dw);

    // Sum the squared differences. No shift needed here as fixed_point_square already handles it.
    return dx_sq + dy_sq + dz_sq + dw_sq;
}

/**
 * nymya_3358_d4_lattice - Applies quantum operations on qubits in a D4 lattice (kernel version).
 * @user_q: Pointer to an array of nymya_qpos4d structures in user space.
 * @count: The number of qubits in the array.
 *
 * This system call simulates quantum operations on qubits arranged in a D4 lattice
 * structure in 4D space. It copies qubit data from user space to kernel space,
 * applies Hadamard gates to each qubit, and then applies CNOT gates between qubits
 * that are within a certain interaction distance (using fixed-point squared distance).
 * Finally, it copies the modified qubit data back to user space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if user_q is NULL or count is less than 24.
 * - -ENOMEM if kernel memory allocation fails.
 * - -EFAULT if copying data between user and kernel space fails.
 */
SYSCALL_DEFINE2(nymya_3358_d4_lattice,
    struct nymya_qpos4d __user *, user_q,
    size_t, count) {

    int ret = 0; // Return value for syscall
    nymya_qpos4d *k_q = NULL; // Kernel-space copy of qubits

    // 1. Validate input arguments
    if (!user_q || count < 24) {
        pr_err("nymya_3358_d4_lattice: Invalid user_q pointer or count (%zu < 24)\n", count);
        return -EINVAL;
    }

    // 2. Allocate kernel memory for qubit data
    k_q = kmalloc_array(count, sizeof(nymya_qpos4d), GFP_KERNEL);
    if (!k_q) {
        pr_err("nymya_3358_d4_lattice: Failed to allocate kernel memory for qubits\n");
        return -ENOMEM;
    }

    // 3. Copy qubit data from user space to kernel space
    if (copy_from_user(k_q, user_q, count * sizeof(nymya_qpos4d))) {
        pr_err("nymya_3358_d4_lattice: Failed to copy qubits from user space\n");
        ret = -EFAULT;
        goto free_k_q; // Jump to cleanup
    }

    // Define the squared epsilon for fixed-point comparison
    // (1.01)^2 = 1.0201. Convert this to fixed-point.
    const int64_t fixed_epsilon_squared = (int64_t)(1.0201 * FIXED_POINT_SCALE);

    // 4. Apply Hadamard gate to each qubit
    for (size_t i = 0; i < count; i++) {
        // Pass the address of the nymya_qubit struct to the hadamard macro/function
        hadamard(&k_q[i].q);
    }

    // 5. Apply CNOT gates between neighboring qubits based on fixed-point squared distance
    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            // Calculate squared distance in fixed-point
            if (dist4d_squared_k(&k_q[i], &k_q[j]) <= fixed_epsilon_squared) {
                // Apply CNOT gate, passing addresses of the qubit structs
                cnot(&k_q[i].q, &k_q[j].q);
            }
        }
    }

    // 6. Log the symbolic event for the D4 lattice entanglement
    // Access struct members directly using '.' operator
    log_symbolic_event("D4_LATTICE", k_q[0].q.id, k_q[0].q.tag, "D4 lattice entangled in 4D");

    // 7. Copy the modified qubits back to user space
    if (copy_to_user(user_q, k_q, count * sizeof(nymya_qpos4d))) {
        pr_err("nymya_3358_d4_lattice: Failed to copy qubits to user space\n");
        ret = -EFAULT;
    }

free_k_q:
    kfree(k_q); // Free allocated kernel memory
    return ret;
}

#endif

