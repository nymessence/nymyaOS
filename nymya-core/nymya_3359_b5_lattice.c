// src/nymya_3359_b5_lattice.c

#include "nymya.h" // Defines: nymya_qubit, nymya_qpos5d, nymya_qpos5d_k, FIXED_POINT_SCALE, fixed_point_square, NYMYA_B5_LATTICE_CODE

#ifndef __KERNEL__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#define __NR_nymya_3359_b5_lattice NYMYA_B5_LATTICE_CODE

/**
 * dist5d - Calculates Euclidean distance in 5D userland.
 */
double dist5d(nymya_qpos5d a, nymya_qpos5d b) {
    return sqrt(pow(a.x-b.x,2)+pow(a.y-b.y,2)
                +pow(a.z-b.z,2)+pow(a.w-b.w,2)
                +pow(a.v-b.v,2));
}

/**
 * nymya_3359_b5_lattice - Userland wrapper for B5 lattice syscall.
 */
int nymya_3359_b5_lattice(nymya_qpos5d q[], size_t count) {
    if (!q || count < 32) return -1;
    nymya_qpos5d_k *buf = malloc(count * sizeof(*buf));
    if (!buf) return -ENOMEM;
    for (size_t i=0;i<count;i++) {
        buf[i].q = q[i].q;
        buf[i].x = (int64_t)(q[i].x*FIXED_POINT_SCALE);
        buf[i].y = (int64_t)(q[i].y*FIXED_POINT_SCALE);
        buf[i].z = (int64_t)(q[i].z*FIXED_POINT_SCALE);
        buf[i].w = (int64_t)(q[i].w*FIXED_POINT_SCALE);
        buf[i].v = (int64_t)(q[i].v*FIXED_POINT_SCALE);
    }
    long ret = syscall(__NR_nymya_3359_b5_lattice,(unsigned long)buf,count);
    if (ret==0) {
        for (size_t i=0;i<count;i++) {
            q[i].q = buf[i].q;
            q[i].x = (double)buf[i].x/ FIXED_POINT_SCALE;
            q[i].y = (double)buf[i].y/ FIXED_POINT_SCALE;
            q[i].z = (double)buf[i].z/ FIXED_POINT_SCALE;
            q[i].w = (double)buf[i].w/ FIXED_POINT_SCALE;
            q[i].v = (double)buf[i].v/ FIXED_POINT_SCALE;
        }
    }
    free(buf);
    return (int)ret;
}

#else // __KERNEL__
    int nymya_3359_b5_lattice_core(nymya_qpos5d_k *k_q, size_t count);


#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/module.h>

static int64_t dist5d_squared_k(const nymya_qpos5d_k *a, const nymya_qpos5d_k *b) {
    int64_t dx=a->x-b->x, dy=a->y-b->y, dz=a->z-b->z;
    int64_t dw=a->w-b->w, dv=a->v-b->v;
    return fixed_point_square(dx)+fixed_point_square(dy)
         + fixed_point_square(dz)+fixed_point_square(dw)
         + fixed_point_square(dv);
}

/**
 * nymya_3359_b5_lattice_core - Kernel core for B5 lattice logic.
 */
int nymya_3359_b5_lattice_core(nymya_qpos5d_k *k_q, size_t count) {
    int ret;
    const int64_t EPS_FP=(int64_t)(1.00*FIXED_POINT_SCALE);
    const int64_t EPS2=fixed_point_square(EPS_FP);
    for (size_t i=0;i<count;i++) {
        ret=hadamard(&k_q[i].q);
        if (ret) return ret;
    }
    for (size_t i=0;i<count;i++) {
        for (size_t j=i+1;j<count;j++) {
            if (dist5d_squared_k(&k_q[i],&k_q[j])<=EPS2) {
                ret=cnot(&k_q[i].q,&k_q[j].q);
                if (ret) return ret;
            }
        }
    }
    log_symbolic_event("B5_LATTICE",k_q[0].q.id,k_q[0].q.tag,
                       "5D B5 lattice entangled");
    return 0;
}
EXPORT_SYMBOL_GPL(nymya_3359_b5_lattice_core);

SYSCALL_DEFINE2(nymya_3359_b5_lattice,
    unsigned long, user_ptr,
    size_t, count) {
    nymya_qpos5d_k *k_q;
    nymya_qpos5d_k __user *u_q=(nymya_qpos5d_k __user*)user_ptr;
    int ret;
    if (!u_q||count<32) return -EINVAL;
    k_q=kmalloc_array(count,sizeof(*k_q),GFP_KERNEL);
    if (!k_q) return -ENOMEM;
    if (copy_from_user(k_q,u_q,count*sizeof(*k_q))) {ret=-EFAULT;goto out;}
    ret=nymya_3359_b5_lattice_core(k_q,count);
    if (!ret) if (copy_to_user(u_q,k_q,count*sizeof(*k_q))) ret=-EFAULT;
out:
    kfree(k_q);
    return ret;
}
#endif