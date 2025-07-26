// src/nymya_3355_fcc_lattice.c
//
// Implements nymya_3355_fcc_lattice syscall: simulates quantum operations on qubits
// arranged in an FCC lattice. Includes both userland and kernel-space versions.

#include "nymya.h" // Defines nymya_qpos3d, nymya_qpos3d_k, FIXED_POINT_SCALE, and __NR_* codes

#ifndef __KERNEL__
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define __NR_nymya_3355_fcc_lattice NYMYA_FCC_LATTICE_CODE

/**
 * nymya_3355_fcc_lattice - Userland wrapper for FCC lattice entanglement.
 * @qubits: array of nymya_qpos3d with length count
 * @count: number of qubits (>=14)
 *
 * Converts to fixed-point, invokes syscall, then rescales results.
 * Returns 0 on success, -1 on invalid input or memory failure, or syscall code.
 */
int nymya_3355_fcc_lattice(nymya_qpos3d qubits[], size_t count) {
    if (!qubits || count < 14) return -1;

    nymya_qpos3d_k *buf = malloc(count * sizeof(*buf));
    if (!buf) return -1;

    // Scale to fixed-point
    for (size_t i = 0; i < count; i++) {
        buf[i].q = qubits[i].q;
        buf[i].x = (int64_t)(qubits[i].x * FIXED_POINT_SCALE);
        buf[i].y = (int64_t)(qubits[i].y * FIXED_POINT_SCALE);
        buf[i].z = (int64_t)(qubits[i].z * FIXED_POINT_SCALE);
    }

    // Syscall
    long ret = syscall(__NR_nymya_3355_fcc_lattice, (unsigned long)buf, count);

    if (ret == 0) {
        // Rescale back
        for (size_t i = 0; i < count; i++) {
            qubits[i].q = buf[i].q;
            qubits[i].x = (double)buf[i].x / FIXED_POINT_SCALE;
            qubits[i].y = (double)buf[i].y / FIXED_POINT_SCALE;
            qubits[i].z = (double)buf[i].z / FIXED_POINT_SCALE;
        }
    }
    free(buf);
    return (int)ret;
}

#else // __KERNEL__
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/errno.h>

static inline int64_t fcc_distance_sq_k(const nymya_qpos3d_k *a,
                                        const nymya_qpos3d_k *b) {
    int64_t dx = a->x - b->x;
    int64_t dy = a->y - b->y;
    int64_t dz = a->z - b->z;
    return fixed_point_square(dx)
         + fixed_point_square(dy)
         + fixed_point_square(dz);
}

/**
 * nymya_3355_fcc_lattice_core - Core FCC lattice logic (kernel).
 * @k_qubits: fixed-point qubit positions array
 * @count: number of qubits
 *
 * Applies Hadamard on each qubit, then CNOT for pairs within threshold.
 */
int nymya_3355_fcc_lattice_core(nymya_qpos3d_k *k_qubits, size_t count) {
    int ret;

    // Hadamard on each
    for (size_t i = 0; i < count; i++) {
        ret = nymya_3308_hadamard_gate(&k_qubits[i].q);
        if (ret) return ret;
    }

    // distance threshold squared
    const int64_t eps2 = (int64_t)(1.0201 * FIXED_POINT_SCALE * FIXED_POINT_SCALE);

    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            if (fcc_distance_sq_k(&k_qubits[i], &k_qubits[j]) <= eps2) {
                ret = nymya_3309_controlled_not(&k_qubits[i].q,
                                                &k_qubits[j].q);
                if (ret) return ret;
            }
        }
    }

    log_symbolic_event("FCC_3D", k_qubits[0].q.id,
                       k_qubits[0].q.tag,
                       "FCC lattice entangled");
    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3355_fcc_lattice_core);

SYSCALL_DEFINE2(nymya_3355_fcc_lattice,
    unsigned long, user_ptr,
    size_t,        count)
{
    nymya_qpos3d_k *k_qubits;
    nymya_qpos3d_k __user *u_qubits = (nymya_qpos3d_k __user *)user_ptr;
    int ret;

    if (!u_qubits || count < 14)
        return -EINVAL;

    k_qubits = kmalloc_array(count, sizeof(*k_qubits), GFP_KERNEL);
    if (!k_qubits)
        return -ENOMEM;

    if (copy_from_user(k_qubits, u_qubits,
                       count * sizeof(*k_qubits))) {
        ret = -EFAULT;
        goto out;
    }

    ret = nymya_3355_fcc_lattice_core(k_qubits, count);
    if (ret) goto out;

    if (copy_to_user(u_qubits, k_qubits,
                     count * sizeof(*k_qubits)))
        ret = -EFAULT;

out:
    kfree(k_qubits);
    return ret;
}

#endif // __KERNEL__

