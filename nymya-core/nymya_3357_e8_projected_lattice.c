// src/nymya_3357_e8_projected_lattice.c

#include "nymya.h" // Defines: nymya_qubit, nymya_qpos3d, nymya_qpos3d_k, FIXED_POINT_SCALE, fixed_point_square, NYMYA_E8_PROJECTED_CODE

#ifndef __KERNEL__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#define __NR_nymya_3357_e8_projected_lattice NYMYA_E8_PROJECTED_CODE

/**
 * e8_distance - Calculates Euclidean distance between two qubit positions.
 */
double e8_distance(nymya_qpos3d a, nymya_qpos3d b) {
    return sqrt(pow(a.x - b.x,2) + pow(a.y - b.y,2) + pow(a.z - b.z,2));
}

/**
 * nymya_3357_e8_projected_lattice - Userland wrapper for E8 projected lattice syscall.
 */
int nymya_3357_e8_projected_lattice(nymya_qpos3d qubits[], size_t count) {
    if (!qubits || count < 30) return -1;
    nymya_qpos3d_k *buf = malloc(count * sizeof(*buf));
    if (!buf) return -ENOMEM;
    for (size_t i=0; i<count; i++) {
        buf[i].q = qubits[i].q;
        buf[i].x = (int64_t)(qubits[i].x * FIXED_POINT_SCALE);
        buf[i].y = (int64_t)(qubits[i].y * FIXED_POINT_SCALE);
        buf[i].z = (int64_t)(qubits[i].z * FIXED_POINT_SCALE);
    }
    long ret = syscall(__NR_nymya_3357_e8_projected_lattice, (unsigned long)buf, count);
    if (ret==0) {
        for (size_t i=0; i<count; i++) {
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
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/module.h>

static int64_t e8_distance_squared_k(const nymya_qpos3d_k *a, const nymya_qpos3d_k *b) {
    int64_t dx=a->x-b->x, dy=a->y-b->y, dz=a->z-b->z;
    return fixed_point_square(dx)+fixed_point_square(dy)+fixed_point_square(dz);
}

/**
 * nymya_3357_e8_projected_lattice_core - Kernel core for E8 projected lattice.
 */
int nymya_3357_e8_projected_lattice_core(nymya_qpos3d_k *k_qubits, size_t count) {
    int ret;
    const int64_t EPS=(int64_t)(1.00*FIXED_POINT_SCALE);
    const int64_t EPS2=fixed_point_square(EPS);
    for (size_t i=0;i<count;i++) {
        ret=hadamard(&k_qubits[i].q);
        if (ret) return ret;
    }
    for (size_t i=0;i<count;i++) {
        for (size_t j=i+1;j<count;j++) {
            if (e8_distance_squared_k(&k_qubits[i],&k_qubits[j])<=EPS2) {
                ret=cnot(&k_qubits[i].q,&k_qubits[j].q);
                if (ret) return ret;
            }
        }
    }
    log_symbolic_event("E8_PROJECTED",k_qubits[0].q.id,k_qubits[0].q.tag,
                       "Projected E8 lattice entangled");
    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3357_e8_projected_lattice_core);

SYSCALL_DEFINE2(nymya_3357_e8_projected_lattice,
    unsigned long,user_ptr,
    size_t,count) {
    nymya_qpos3d_k *k_qubits;
    nymya_qpos3d_k __user *u_qubits=(nymya_qpos3d_k __user*)user_ptr;
    int ret;
    if (!u_qubits||count<30) return -EINVAL;
    k_qubits=kmalloc_array(count,sizeof(*k_qubits),GFP_KERNEL);
    if (!k_qubits) return -ENOMEM;
    if (copy_from_user(k_qubits,u_qubits,count*sizeof(*k_qubits))) {ret=-EFAULT;goto out;}
    ret=nymya_3357_e8_projected_lattice_core(k_qubits,count);
    if (!ret) if (copy_to_user(u_qubits,k_qubits,count*sizeof(*k_qubits))) ret=-EFAULT;
out:
    kfree(k_qubits);
    return ret;
}
#endif

