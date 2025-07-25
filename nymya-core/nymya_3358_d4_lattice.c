// src/nymya_3358_d4_lattice.c

#include "nymya.h" // Defines: nymya_qubit, nymya_qpos4d, nymya_qpos4d_k, FIXED_POINT_SCALE, fixed_point_square, NYMYA_D4_LATTICE_CODE

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
#define __NR_nymya_3358_d4_lattice NYMYA_D4_LATTICE_CODE

// Forward declarations for userland functions.
// hadamard, cnot, and log_symbolic_event are assumed to be declared in nymya.h.


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
 * nymya_3358_d4_lattice - Userland wrapper for the D4 lattice syscall.
 * @q: An array of nymya_qpos4d structures, representing qubits with their 4D positions.
 * @count: The number of qubits in the array.
 *
 * This function converts userland double-precision coordinates to kernel-friendly
 * fixed-point representation, invokes the kernel syscall, and then converts
 * the results back to double-precision if the kernel modified positions.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubits array is NULL or count is less than 24.
 * - -ENOMEM if memory allocation fails.
 * - Other negative values from the kernel syscall.
 */
int nymya_3358_d4_lattice(nymya_qpos4d q[], size_t count) {
    // Check for valid input array and minimum count for D4 lattice
    if (!q || count < 24) return -1;

    // Allocate buffer for fixed-point kernel structures
    nymya_qpos4d_k *buf = malloc(count * sizeof(*buf));
    if (!buf) return -ENOMEM;

    // Scale to fixed-point for kernel
    for (size_t i = 0; i < count; i++) {
        buf[i].q = q[i].q; // Copy the qubit structure
        buf[i].x = (int64_t)(q[i].x * FIXED_POINT_SCALE);
        buf[i].y = (int64_t)(q[i].y * FIXED_POINT_SCALE);
        buf[i].z = (int64_t)(q[i].z * FIXED_POINT_SCALE);
        buf[i].w = (int64_t)(q[i].w * FIXED_POINT_SCALE);
    }

    // Perform the syscall directly
    long ret = syscall(__NR_nymya_3358_d4_lattice, (unsigned long)buf, count);

    if (ret == 0) {
        // Rescale back to doubles if kernel modified positions
        for (size_t i = 0; i < count; i++) {
            q[i].q = buf[i].q; // Copy the qubit structure back
            q[i].x = (double)buf[i].x / FIXED_POINT_SCALE;
            q[i].y = (double)buf[i].y / FIXED_POINT_SCALE;
            q[i].z = (double)buf[i].z / FIXED_POINT_SCALE;
            q[i].w = (double)buf[i].w / FIXED_POINT_SCALE;
        }
    }

    free(buf);
    return (int)ret;
}

#else // __KERNEL__

// External kernel functions for qubit operations and logging.
// These are assumed to be defined elsewhere in the kernel and linked,
// and their declarations are now expected to be in nymya.h.


/**
 * dist4d_squared_k - Calculates the squared Euclidean distance between two
 * 4D fixed-point positions in kernel space.
 * @a: Pointer to the first 4D fixed-point position (nymya_qpos4d_k).
 * @b: Pointer to the second 4D fixed-point position (nymya_qpos4d_k).
 *
 * This function computes the squared Euclidean distance between two points
 * in 4D space using fixed-point arithmetic directly on the fixed-point coordinates.
 *
 * Returns:
 * The squared distance as an int64_t (fixed-point).
 */
static int64_t dist4d_squared_k(const nymya_qpos4d_k *a, const nymya_qpos4d_k *b) {
    // Calculate differences in fixed-point.
    // The 'x', 'y', 'z', 'w' members of nymya_qpos4d_k are already fixed-point integers.
    int64_t dx = a->x - b->x;
    int64_t dy = a->y - b->y;
    int64_t dz = a->z - b->z;
    int64_t dw = a->w - b->w;

    // Calculate squared differences in fixed-point using fixed_point_square from nymya.h
    int64_t dx_sq = fixed_point_square(dx);
    int64_t dy_sq = fixed_point_square(dy);
    int64_t dz_sq = fixed_point_square(dz);
    int64_t dw_sq = fixed_point_square(dw);

    // Sum the squared differences.
    return dx_sq + dy_sq + dz_sq + dw_sq;
}

/**
 * nymya_3358_d4_lattice - Applies quantum operations on qubits in a D4 lattice (kernel version).
 * @user_ptr: Userland pointer to an array of nymya_qpos4d_k structures.
 * @count: The number of qubits in the array.
 *
 * This system call simulates quantum operations on qubits arranged in a D4 lattice
 * structure in 4D space. It copies qubit data from user space to kernel space
 * (converting to fixed-point if necessary, handled by userland wrapper),
 * applies Hadamard gates to each qubit, and then applies CNOT gates between qubits
 * that are within a certain interaction distance (using fixed-point squared distance).
 * Finally, it copies the modified qubit data back to user space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if user_ptr is NULL or count is less than 24.
 * - -ENOMEM if kernel memory allocation fails.
 * - -EFAULT if copying data between user and kernel space fails.
 */
SYSCALL_DEFINE2(nymya_3358_d4_lattice,
    unsigned long, user_ptr, // Changed to unsigned long to match syscall signature
    size_t, count) {

    int ret = 0; // Return value for syscall
    nymya_qpos4d_k *k_q = NULL; // Kernel-space copy of qubits (fixed-point)
    nymya_qpos4d_k __user *u_q = (nymya_qpos4d_k __user *)user_ptr; // Cast user_ptr

    // 1. Validate input arguments
    if (!u_q || count < 24) {
        pr_err("nymya_3358_d4_lattice: Invalid user_ptr or count (%zu < 24)\n", count);
        return -EINVAL;
    }

    // 2. Allocate kernel memory for qubit data
    k_q = kmalloc_array(count, sizeof(nymya_qpos4d_k), GFP_KERNEL); // Allocate for fixed-point type
    if (!k_q) {
        pr_err("nymya_3358_d4_lattice: Failed to allocate kernel memory for qubits\n");
        return -ENOMEM;
    }

    // 3. Copy qubit data from user space to kernel space
    if (copy_from_user(k_q, u_q, count * sizeof(nymya_qpos4d_k))) { // Copy fixed-point type
        pr_err("nymya_3358_d4_lattice: Failed to copy qubits from user space\n");
        ret = -EFAULT;
        goto free_k_q; // Jump to cleanup
    }

    // Define the squared epsilon for fixed-point comparison
    // (1.01)^2 = 1.0201. Convert this to fixed-point.
    const int64_t EPSILON_FP = (int64_t)(1.01 * FIXED_POINT_SCALE);
    const int64_t fixed_epsilon_squared = fixed_point_square(EPSILON_FP);

    // 4. Apply Hadamard gate to each qubit
    for (size_t i = 0; i < count; i++) {
        // Pass the address of the nymya_qubit struct to the hadamard function
        ret = hadamard(&k_q[i].q); // Assuming hadamard returns int
        if (ret) {
            pr_err("nymya_3358_d4_lattice: Hadamard gate failed for qubit %llu\n", k_q[i].q.id);
            goto free_k_q;
        }
    }

    // 5. Apply CNOT gates between neighboring qubits based on fixed-point squared distance
    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            // Calculate squared distance in fixed-point
            if (dist4d_squared_k(&k_q[i], &k_q[j]) <= fixed_epsilon_squared) {
                // Apply CNOT gate, passing addresses of the qubit structs
                ret = cnot(&k_q[i].q, &k_q[j].q); // Assuming cnot returns int
                if (ret) {
                    pr_err("nymya_3358_d4_lattice: CNOT gate failed between qubits %llu and %llu\n",
                           k_q[i].q.id, k_q[j].q.id);
                    goto free_k_q;
                }
            }
        }
    }

    // 6. Log the symbolic event for the D4 lattice entanglement
    log_symbolic_event("D4_LATTICE", k_q[0].q.id, k_q[0].q.tag, "D4 lattice entangled in 4D");

    // 7. Copy the modified qubits back to user space
    if (copy_to_user(u_q, k_q, count * sizeof(nymya_qpos4d_k))) { // Copy fixed-point type
        pr_err("nymya_3358_d4_lattice: Failed to copy qubits to user space\n");
        ret = -EFAULT;
    }

free_k_q:
    kfree(k_q); // Free allocated kernel memory
    return ret;
}

#endif

