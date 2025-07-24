// src/nymya_3356_hcp_lattice.c

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
 * hcp_distance - Calculates the Euclidean distance between two nymya_qpos3d points (userland version).
 * @a: The first 3D qubit position.
 * @b: The second 3D qubit position.
 *
 * This function computes the standard Euclidean distance between two points
 * in 3D space using floating-point arithmetic.
 *
 * Returns:
 * The distance as a double.
 */
double hcp_distance(nymya_qpos3d a, nymya_qpos3d b) {
    return sqrt(pow(a.x - b.x, 2) +
                pow(a.y - b.y, 2) +
                pow(a.z - b.z, 2));
}

/**
 * nymya_3356_hcp_lattice - Applies quantum operations on qubits in an HCP lattice (userland version).
 * @qubits: An array of nymya_qpos3d structures, representing qubits with their 3D positions.
 * @count: The number of qubits in the array.
 *
 * This function simulates quantum operations on qubits arranged in a Hexagonal
 * Close-Packed (HCP) lattice structure. It initializes each qubit with a Hadamard gate
 * and then applies CNOT gates between qubits that are within a certain interaction
 * distance (epsilon).
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubits array is NULL or count is less than 17 (minimum for a meaningful HCP unit).
 */
int nymya_3356_hcp_lattice(nymya_qpos3d qubits[], size_t count) {
    // Check for valid input array and minimum count for HCP lattice
    if (!qubits || count < 17) return -1;

    // Epsilon defines the interaction radius for CNOT gates.
    // Qubits within this distance are considered "neighbors" for entanglement.
    const double epsilon = 1.01;

    // Apply Hadamard gate to each qubit to put them in superposition
    for (size_t i = 0; i < count; i++) {
        // qubits[i].q is a nymya_qubit struct, pass its address to hadamard
        hadamard(&qubits[i].q);
    }

    // Apply CNOT gates between neighboring qubits based on distance
    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            // Calculate distance between qubit positions
            if (hcp_distance(qubits[i], qubits[j]) <= epsilon) {
                // Apply CNOT gate, passing addresses of the qubit structs
                cnot(&qubits[i].q, &qubits[j].q);
            }
        }
    }

    // Log the symbolic event for the HCP lattice entanglement
    // Access struct members directly using '.' operator
    log_symbolic_event("HCP_3D", qubits[0].q.id, qubits[0].q.tag, "HCP lattice entangled");
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
 * hcp_distance_squared_k - Calculates the squared Euclidean distance between two nymya_qpos3d points (kernel version).
 * @a: Pointer to the first 3D qubit position.
 * @b: Pointer to the second 3D qubit position.
 *
 * This function computes the squared Euclidean distance between two points
 * in 3D space using fixed-point arithmetic. It avoids floating-point operations
 * which are generally disallowed in kernel code.
 *
 * Returns:
 * The squared distance as an int64_t (fixed-point).
 */
static int64_t hcp_distance_squared_k(const nymya_qpos3d *a, const nymya_qpos3d *b) {
    // Calculate differences in fixed-point
    int64_t dx = (int64_t)((a->x - b->x) * FIXED_POINT_SCALE);
    int64_t dy = (int64_t)((a->y - b->y) * FIXED_POINT_SCALE);
    int64_t dz = (int64_t)((a->z - b->z) * FIXED_POINT_SCALE);

    // Calculate squared differences in fixed-point
    int64_t dx_sq = fixed_point_square(dx);
    int64_t dy_sq = fixed_point_square(dy);
    int64_t dz_sq = fixed_point_square(dz);

    // Sum the squared differences. No shift needed here as fixed_point_square already handles it.
    return dx_sq + dy_sq + dz_sq;
}

/**
 * nymya_3356_hcp_lattice - Applies quantum operations on qubits in an HCP lattice (kernel version).
 * @user_qubits: Pointer to an array of nymya_qpos3d structures in user space.
 * @count: The number of qubits in the array.
 *
 * This system call simulates quantum operations on qubits arranged in a Hexagonal
 * Close-Packed (HCP) lattice structure. It copies qubit data from user space to kernel space,
 * applies Hadamard gates to each qubit, and then applies CNOT gates between qubits
 * that are within a certain interaction distance (using fixed-point squared distance).
 * Finally, it copies the modified qubit data back to user space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if user_qubits is NULL or count is less than 17.
 * - -ENOMEM if kernel memory allocation fails.
 * - -EFAULT if copying data between user and kernel space fails.
 */
SYSCALL_DEFINE2(nymya_3356_hcp_lattice,
    struct nymya_qpos3d __user *, user_qubits,
    size_t, count) {

    int ret = 0; // Return value for syscall
    nymya_qpos3d *k_qubits = NULL; // Kernel-space copy of qubits

    // 1. Validate input arguments
    if (!user_qubits || count < 17) {
        pr_err("nymya_3356_hcp_lattice: Invalid user_qubits pointer or count (%zu < 17)\n", count);
        return -EINVAL;
    }

    // 2. Allocate kernel memory for qubit data
    k_qubits = kmalloc_array(count, sizeof(nymya_qpos3d), GFP_KERNEL);
    if (!k_qubits) {
        pr_err("nymya_3356_hcp_lattice: Failed to allocate kernel memory for qubits\n");
        return -ENOMEM;
    }

    // 3. Copy qubit data from user space to kernel space
    if (copy_from_user(k_qubits, user_qubits, count * sizeof(nymya_qpos3d))) {
        pr_err("nymya_3356_hcp_lattice: Failed to copy qubits from user space\n");
        ret = -EFAULT;
        goto free_k_qubits; // Jump to cleanup
    }

    // Define the squared epsilon for fixed-point comparison
    // (1.01)^2 = 1.0201. Convert this to fixed-point.
    const int64_t fixed_epsilon_squared = (int64_t)(1.0201 * FIXED_POINT_SCALE);

    // 4. Apply Hadamard gate to each qubit
    for (size_t i = 0; i < count; i++) {
        // Pass the address of the nymya_qubit struct to the hadamard macro/function
        hadamard(&k_qubits[i].q);
    }

    // 5. Apply CNOT gates between neighboring qubits based on fixed-point squared distance
    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            // Calculate squared distance in fixed-point
            if (hcp_distance_squared_k(&k_qubits[i], &k_qubits[j]) <= fixed_epsilon_squared) {
                // Apply CNOT gate, passing addresses of the qubit structs
                cnot(&k_qubits[i].q, &k_qubits[j].q);
            }
        }
    }

    // 6. Log the symbolic event for the HCP lattice entanglement
    // Access struct members directly using '.' operator
    log_symbolic_event("HCP_3D", k_qubits[0].q.id, k_qubits[0].q.tag, "HCP lattice entangled");

    // 7. Copy the modified qubits back to user space
    if (copy_to_user(user_qubits, k_qubits, count * sizeof(nymya_qpos3d))) {
        pr_err("nymya_3356_hcp_lattice: Failed to copy qubits to user space\n");
        ret = -EFAULT;
    }

free_k_qubits:
    kfree(k_qubits); // Free allocated kernel memory
    return ret;
}

#endif

