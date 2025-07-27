// src/nymya_3356_hcp_lattice.c

#include "nymya.h" // Defines: nymya_qubit, nymya_qpos3d, nymya_qpos3d_k, FIXED_POINT_SCALE, fixed_point_square, NYMYA_HCP_LATTICE_CODE

#ifndef __KERNEL__
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#define __NR_nymya_3356_hcp_lattice NYMYA_HCP_LATTICE_CODE

/**
 * hcp_distance - Calculates Euclidean distance between two qubit positions.
 */
double hcp_distance(nymya_qpos3d a, nymya_qpos3d b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

/**
 * nymya_3356_hcp_lattice - Userland wrapper for HCP lattice syscall.
 */
int nymya_3356_hcp_lattice(nymya_qpos3d qubits[], size_t count) {
    if (!qubits || count < 17) return -1;
    nymya_qpos3d_k *buf = malloc(count * sizeof(*buf));
    if (!buf) return -ENOMEM;
    for (size_t i = 0; i < count; i++) {
        buf[i].q = qubits[i].q;
        buf[i].x = (int64_t)(qubits[i].x * FIXED_POINT_SCALE);
        buf[i].y = (int64_t)(qubits[i].y * FIXED_POINT_SCALE);
        buf[i].z = (int64_t)(qubits[i].z * FIXED_POINT_SCALE);
    }
    long ret = syscall(__NR_nymya_3356_hcp_lattice, (unsigned long)buf, count);
    if (ret == 0) {
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
    int nymya_3356_hcp_lattice_core(nymya_qpos3d_k *k_qubits, size_t count);


#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/module.h>

static int64_t hcp_distance_squared_k(const nymya_qpos3d_k *a,
                                        const nymya_qpos3d_k *b) {
    int64_t dx = a->x - b->x;
    int64_t dy = a->y - b->y;
    int64_t dz = a->z - b->z;
    return fixed_point_square(dx) + fixed_point_square(dy) + fixed_point_square(dz);
}

/**
 * nymya_3356_hcp_lattice_core - Kernel core for HCP lattice operations.
 */
int nymya_3356_hcp_lattice_core(nymya_qpos3d_k *k_qubits, size_t count) {
    int ret;
    const int64_t EPS_FP = (int64_t)(1.01 * FIXED_POINT_SCALE);
    const int64_t EPS2 = fixed_point_square(EPS_FP);
    for (size_t i = 0; i < count; i++) {
        ret = nymya_3308_hadamard_gate(&k_qubits[i].q);
        if (ret) return ret;
    }
    for (size_t i = 0; i < count; i++) {
        for (size_t j = i+1; j < count; j++) {
            if (hcp_distance_squared_k(&k_qubits[i], &k_qubits[j]) <= EPS2) {
                ret = nymya_3309_controlled_not(&k_qubits[i].q, &k_qubits[j].q);
                if (ret) return ret;
            }
        }
    }
    log_symbolic_event("HCP_3D", k_qubits[0].q.id, k_qubits[0].q.tag, "HCP lattice entangled");
    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3356_hcp_lattice_core);

SYSCALL_DEFINE2(nymya_3356_hcp_lattice,
    unsigned long, user_ptr,
    size_t, count) {
    nymya_qpos3d_k *k_qubits;
    nymya_qpos3d_k __user *u_qubits = (nymya_qpos3d_k __user *)user_ptr;
    int ret;
    if (!u_qubits || count < 17)
        return -EINVAL;
    k_qubits = kmalloc_array(count, sizeof(*k_qubits), GFP_KERNEL);
    if (!k_qubits) return -ENOMEM;
    if (copy_from_user(k_qubits, u_qubits, count * sizeof(*k_qubits))) {
        ret = -EFAULT;
        goto out;
    }
    ret = nymya_3356_hcp_lattice_core(k_qubits, count);
    if (!ret)
        if (copy_to_user(u_qubits, k_qubits, count * sizeof(*k_qubits)))
            ret = -EFAULT;
out:
    kfree(k_qubits);
    return ret;
}
#endif // __KERNEL__