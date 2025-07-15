// src/nymya_3355_fcc_lattice.c

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/slab.h>
#endif

#ifndef __KERNEL__

double fcc_distance(nymya_qpos3d a, nymya_qpos3d b) {
    return sqrt(pow(a.x - b.x, 2) +
                pow(a.y - b.y, 2) +
                pow(a.z - b.z, 2));
}

int nymya_3355_fcc_lattice(nymya_qpos3d qubits[], size_t count) {
    if (!qubits || count < 14) return -1;

    const double epsilon = 1.01;

    for (size_t i = 0; i < count; i++) {
        if (!qubits[i].q) return -1;
        hadamard(qubits[i].q);
    }

    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            if (fcc_distance(qubits[i], qubits[j]) <= epsilon) {
                cnot(qubits[i].q, qubits[j].q);
            }
        }
    }

    log_symbolic_event("FCC_3D", qubits[0].q->id, qubits[0].q->tag, "FCC lattice entangled");
    return 0;
}

#else

static double fcc_distance_k(const nymya_qpos3d *a, const nymya_qpos3d *b) {
    return sqrt(pow(a->x - b->x, 2) +
                pow(a->y - b->y, 2) +
                pow(a->z - b->z, 2));
}

SYSCALL_DEFINE2(nymya_3355_fcc_lattice,
    struct nymya_qpos3d __user *, user_qubits,
    size_t, count) {

    if (!user_qubits || count < 14)
        return -EINVAL;

    nymya_qpos3d *k_qubits = kmalloc_array(count, sizeof(nymya_qpos3d), GFP_KERNEL);
    if (!k_qubits)
        return -ENOMEM;

    if (copy_from_user(k_qubits, user_qubits, count * sizeof(nymya_qpos3d))) {
        kfree(k_qubits);
        return -EFAULT;
    }

    // Validate pointers inside k_qubits
    for (size_t i = 0; i < count; i++) {
        if (!k_qubits[i].q) {
            kfree(k_qubits);
            return -EINVAL;
        }
    }

    for (size_t i = 0; i < count; i++) {
        hadamard(k_qubits[i].q);
    }

    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            if (fcc_distance_k(&k_qubits[i], &k_qubits[j]) <= 1.01) {
                cnot(k_qubits[i].q, k_qubits[j].q);
            }
        }
    }

    log_symbolic_event("FCC_3D", k_qubits[0].q->id, k_qubits[0].q->tag, "FCC lattice entangled");

    // Write back if needed (positions unchanged, but qubit states changed)
    if (copy_to_user(user_qubits, k_qubits, count * sizeof(nymya_qpos3d))) {
        kfree(k_qubits);
        return -EFAULT;
    }

    kfree(k_qubits);
    return 0;
}

#endif
