// src/nymya_3357_e8_projected_lattice.c

#include "nymya.h" // Defines: nymya_qubit, nymya_qpos3d, nymya_qpos3d_k, FIXED_POINT_SCALE, fixed_point_square, NYMYA_E8_PROJECTED_CODE

#ifndef __KERNEL__
    #include <stdio.h>
    #include <stdlib.h> // For malloc, free in userland
    #include <math.h>   // Include math.h for userspace for sqrt and pow
    #include <unistd.h> // For syscall
    #include <sys/syscall.h> // For syscall() function prototype
    #include <errno.h>  // For ENOMEM and other errno values in userland
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/slab.h> // For kmalloc_array, kfree
    // No math.h for kernel, fixed-point math assumed or implemented via nymya.h
#endif

#ifndef __KERNEL__

// Define the syscall number for userland, using the code from nymya.h.
// This is necessary because syscall() expects the __NR_ prefix.
#define __NR_nymya_3357_e8_projected_lattice NYMYA_E8_PROJECTED_CODE

// Forward declarations for userland functions.
// hadamard, cnot, and log_symbolic_event are now assumed to be declared in nymya.h.


/**
 * e8_distance - Calculates the Euclidean distance between two nymya_qpos3d points (userland version).
 * @a: The first 3D qubit position.
 * @b: The second 3D qubit position.
 *
 * This function computes the standard Euclidean distance between two points
 * in 3D space using floating-point arithmetic.
 *
 * Returns:
 * The distance as a double.
 */
double e8_distance(nymya_qpos3d a, nymya_qpos3d b) {
    return sqrt(pow(a.x - b.x, 2) +
                pow(a.y - b.y, 2) +
                pow(a.z - b.z, 2));
}

/**
 * nymya_3357_e8_projected_lattice - Userland wrapper for the E8 projected lattice syscall.
 * @qubits: An array of nymya_qpos3d structures, representing qubits with their 3D positions.
 * @count: The number of qubits in the array.
 *
 * This function converts userland double-precision coordinates to kernel-friendly
 * fixed-point representation, invokes the kernel syscall, and then converts
 * the results back to double-precision if the kernel modified positions.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubits array is NULL or count is less than 30.
 * - -ENOMEM if memory allocation fails.
 * - Other negative values from the kernel syscall.
 */
int nymya_3357_e8_projected_lattice(nymya_qpos3d qubits[], size_t count) {
    if (!qubits || count < 30) return -1;

    // Allocate buffer for fixed-point kernel structures
    nymya_qpos3d_k *buf = malloc(count * sizeof(*buf));
    if (!buf) return -ENOMEM;

    // Scale to fixed-point for kernel
    for (size_t i = 0; i < count; i++) {
        buf[i].q = qubits[i].q; // Copy the qubit structure
        buf[i].x = (int64_t)(qubits[i].x * FIXED_POINT_SCALE);
        buf[i].y = (int64_t)(qubits[i].y * FIXED_POINT_SCALE);
        buf[i].z = (int64_t)(qubits[i].z * FIXED_POINT_SCALE);
    }

    // Perform the syscall directly
    long ret = syscall(__NR_nymya_3357_e8_projected_lattice, (unsigned long)buf, count);

    if (ret == 0) {
        // Rescale back to doubles if kernel modified positions
        for (size_t i = 0; i < count; i++) {
            qubits[i].q = buf[i].q; // Copy the qubit structure back
            qubits[i].x = (double)buf[i].x / FIXED_POINT_SCALE;
            qubits[i].y = (double)buf[i].y / FIXED_POINT_SCALE;
            qubits[i].z = (double)buf[i].z / FIXED_POINT_SCALE;
        }
    }

    free(buf);
    return (int)ret;
}

#else // __KERNEL__

// External kernel functions for qubit operations and logging.
// These are assumed to be defined elsewhere in the kernel and linked,
// and their declarations are now expected to be in nymya.h.
// No explicit extern declarations needed here if they are in nymya.h.


/**
 * e8_distance_squared_k - Calculates the squared Euclidean distance between two
 * 3D fixed-point positions in kernel space.
 * @a: Pointer to the first 3D fixed-point position (nymya_qpos3d_k).
 * @b: Pointer to the second 3D fixed-point position (nymya_qpos3d_k).
 *
 * This function computes the squared Euclidean distance between two points
 * in 3D space using fixed-point arithmetic directly on the fixed-point coordinates.
 *
 * Returns:
 * The squared distance as an int64_t (fixed-point).
 */
static int64_t e8_distance_squared_k(const nymya_qpos3d_k *a, const nymya_qpos3d_k *b) {
    // Calculate differences in fixed-point.
    // The 'x', 'y', 'z' members of nymya_qpos3d_k are already fixed-point integers.
    int64_t dx = a->x - b->x;
    int64_t dy = a->y - b->y;
    int64_t dz = a->z - b->z;

    // Calculate squared differences in fixed-point using fixed_point_square from nymya.h
    int64_t dx_sq = fixed_point_square(dx);
    int64_t dy_sq = fixed_point_square(dy);
    int64_t dz_sq = fixed_point_square(dz);

    // Sum the squared differences.
    return dx_sq + dy_sq + dz_sq;
}

/**
 * nymya_3357_e8_projected_lattice - Applies quantum operations on qubits in an E8 projected lattice (kernel version).
 * @user_ptr: Userland pointer to an array of nymya_qpos3d_k structures.
 * @count: The number of qubits in the array.
 *
 * This system call simulates quantum operations on qubits arranged in a 3D projection
 * of an E8 lattice structure. It copies qubit data from user space to kernel space
 * (converting to fixed-point if necessary, handled by userland wrapper),
 * applies Hadamard gates to each qubit, and then applies CNOT gates between qubits
 * that are within a certain interaction distance (using fixed-point squared distance).
 * Finally, it copies the modified qubit data back to user space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if user_ptr is NULL or count is less than 30.
 * - -ENOMEM if kernel memory allocation fails.
 * - -EFAULT if copying data between user and kernel space fails.
 */
SYSCALL_DEFINE2(nymya_3357_e8_projected_lattice,
    unsigned long, user_ptr, // Changed to unsigned long to match syscall signature
    size_t, count) {

    int ret = 0; // Return value for syscall
    nymya_qpos3d_k *k_qubits = NULL; // Kernel-space copy of qubits (fixed-point)
    nymya_qpos3d_k __user *u_qubits = (nymya_qpos3d_k __user *)user_ptr; // Cast user_ptr

    // 1. Validate input arguments
    if (!u_qubits || count < 30) {
        pr_err("nymya_3357_e8_projected_lattice: Invalid user_ptr or count (%zu < 30)\n", count);
        return -EINVAL;
    }

    // 2. Allocate kernel memory for qubit data
    k_qubits = kmalloc_array(count, sizeof(nymya_qpos3d_k), GFP_KERNEL); // Allocate for fixed-point type
    if (!k_qubits) {
        pr_err("nymya_3357_e8_projected_lattice: Failed to allocate kernel memory for qubits\n");
        return -ENOMEM;
    }

    // 3. Copy qubit data from user space to kernel space
    if (copy_from_user(k_qubits, u_qubits, count * sizeof(nymya_qpos3d_k))) { // Copy fixed-point type
        pr_err("nymya_3357_e8_projected_lattice: Failed to copy qubits from user space\n");
        ret = -EFAULT;
        goto free_k_qubits; // Jump to cleanup
    }

    // Define the squared epsilon for fixed-point comparison
    // (1.00)^2 = 1.00. Convert this to fixed-point.
    const int64_t EPSILON_FP = (int64_t)(1.00 * FIXED_POINT_SCALE);
    const int64_t fixed_epsilon_squared = fixed_point_square(EPSILON_FP);

    // 4. Apply Hadamard gate to each qubit
    for (size_t i = 0; i < count; i++) {
        // Pass the address of the nymya_qubit struct to the hadamard function
        ret = hadamard(&k_qubits[i].q); // Assuming hadamard returns int
        if (ret) {
            pr_err("nymya_3357_e8_projected_lattice: Hadamard gate failed for qubit %llu\n", k_qubits[i].q.id);
            goto free_k_qubits;
        }
    }

    // 5. Apply CNOT gates between neighboring qubits based on fixed-point squared distance
    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            // Calculate squared distance in fixed-point
            if (e8_distance_squared_k(&k_qubits[i], &k_qubits[j]) <= fixed_epsilon_squared) {
                // Apply CNOT gate, passing addresses of the qubit structs
                ret = cnot(&k_qubits[i].q, &k_qubits[j].q); // Assuming cnot returns int
                if (ret) {
                    pr_err("nymya_3357_e8_projected_lattice: CNOT gate failed between qubits %llu and %llu\n",
                           k_qubits[i].q.id, k_qubits[j].q.id);
                    goto free_k_qubits;
                }
            }
        }
    }

    // 6. Log the symbolic event for the E8 projected lattice entanglement
    log_symbolic_event("E8_PROJECTED", k_qubits[0].q.id, k_qubits[0].q.tag,
        "Projected E8 lattice entanglement");

    // 7. Copy the modified qubits back to user space
    if (copy_to_user(u_qubits, k_qubits, count * sizeof(nymya_qpos3d_k))) { // Copy fixed-point type
        pr_err("nymya_3357_e8_projected_lattice: Failed to copy qubits to user space\n");
        ret = -EFAULT;
    }

free_k_qubits:
    kfree(k_qubits); // Free allocated kernel memory
    return ret;
}

#endif

