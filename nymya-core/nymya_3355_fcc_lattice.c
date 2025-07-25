// src/nymya_3355_fcc_lattice.c
//
// Implements nymya_3355_fcc_lattice syscall: simulates quantum operations on qubits
// arranged in an FCC lattice. Userland wrapper scales to fixed-point and invokes
// the kernel syscall; kernel branch uses integer-only math.

#include "nymya.h"    // Defines: nymya_qubit, nymya_qpos3d, nymya_qpos3d_k, FIXED_POINT_SCALE

#ifndef __KERNEL__
    // Ensure syscall number is defined before including headers
    #ifndef __NR_nymya_3355_fcc_lattice
    #define __NR_nymya_3355_fcc_lattice 333  // adjust to real syscall number
    #endif

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <unistd.h>
    #include <sys/syscall.h>
    #include <sys/types.h>

    /**
     * Userland wrapper: converts doubles to fixed-point, calls syscall, rescales.
     */
    int nymya_3355_fcc_lattice(nymya_qpos3d qubits[], size_t count) {
        if (!qubits || count < 14) return -1;

        // Use fixed-point struct from nymya.h
        nymya_qpos3d_k *buf = malloc(count * sizeof(*buf));
        if (!buf) return -1;

        // Scale to fixed-point
        for (size_t i = 0; i < count; i++) {
            buf[i].q = qubits[i].q;
            buf[i].x = (int64_t)(qubits[i].x * FIXED_POINT_SCALE);
            buf[i].y = (int64_t)(qubits[i].y * FIXED_POINT_SCALE);
            buf[i].z = (int64_t)(qubits[i].z * FIXED_POINT_SCALE);
        }

        // Perform the syscall directly
        long ret = syscall(__NR_nymya_3355_fcc_lattice, (unsigned long)buf, count);

        if (ret == 0) {
            // Rescale back to doubles if kernel modified positions
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

    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/slab.h>
    #include <linux/types.h>

    static inline int64_t fcc_distance_sq_k(const nymya_qpos3d_k *a,
                                            const nymya_qpos3d_k *b) {
        int64_t dx = a->x - b->x;
        int64_t dy = a->y - b->y;
        int64_t dz = a->z - b->z;
        return fixed_point_square(dx)
             + fixed_point_square(dy)
             + fixed_point_square(dz);
    }

    SYSCALL_DEFINE2(nymya_3355_fcc_lattice,
        unsigned long, user_ptr,
        size_t,        count)
    {
        int ret = 0;
        nymya_qpos3d_k *k_qubits;
        nymya_qpos3d_k __user *u_qubits = (nymya_qpos3d_k __user *)user_ptr;

        if (!u_qubits || count < 14)
            return -EINVAL;

        k_qubits = kmalloc_array(count, sizeof(*k_qubits), GFP_KERNEL);
        if (!k_qubits)
            return -ENOMEM;

        if (copy_from_user(k_qubits, u_qubits,
                           count * sizeof(*k_qubits))) {
            ret = -EFAULT;
            goto cleanup;
        }

        for (size_t i = 0; i < count; i++) {
            ret = nymya_3308_hadamard_gate(&k_qubits[i].q);
            if (ret) goto cleanup;
        }

        const int64_t eps2 = (int64_t)(1.0201 * FIXED_POINT_SCALE * FIXED_POINT_SCALE);

        for (size_t i = 0; i < count; i++) {
            for (size_t j = i + 1; j < count; j++) {
                if (fcc_distance_sq_k(&k_qubits[i], &k_qubits[j]) <= eps2) {
                    ret = nymya_3309_controlled_not(&k_qubits[i].q,
                                                    &k_qubits[j].q);
                    if (ret) goto cleanup;
                }
            }
        }

        log_symbolic_event("FCC_3D", k_qubits[0].q.id,
                           k_qubits[0].q.tag,
                           "FCC lattice entangled");

        if (!ret) {
            if (copy_to_user(u_qubits, k_qubits,
                             count * sizeof(*k_qubits)))
                ret = -EFAULT;
        }

    cleanup:
        kfree(k_qubits);
        return ret;
    }

#endif

