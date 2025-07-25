// src/nymya_3307_sqrt_x_gate.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_nymya_3307_sqrt_x_gate NYMYA_SQRT_X_CODE

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
    #include <linux/errno.h>
    #include <linux/module.h> // Required for EXPORT_SYMBOL_GPL

    /**
     * nymya_3307_sqrt_x_gate - Core kernel function for Square Root X gate logic.
     * @q: Pointer to the nymya_qubit structure.
     *
     * This function applies the Square Root X gate logic to a qubit's amplitude
     * using fixed-point arithmetic. It's equivalent to multiplying by (1/√2)(1 + i).
     * This function is designed to be called directly by other kernel code.
     *
     * Returns: 0 on success.
     */
    int nymya_3307_sqrt_x_gate(struct nymya_qubit *q) {
        int64_t re, im;
        // fixed-point approximation of 1/sqrt(2)
        const int64_t c_re = (int64_t)(0.70710678 * FIXED_POINT_SCALE);
        const int64_t c_im = (int64_t)(0.70710678 * FIXED_POINT_SCALE);

        if (!q) {
            pr_err("NYMYA: nymya_3307_sqrt_x_gate received NULL qubit pointer\n");
            return -EINVAL;
        }

        re = q->amplitude.re;
        im = q->amplitude.im;

        // Fixed-point complex multiplication:
        // new_re = (re*c_re - im*c_im) / FIXED_POINT_SCALE
        // new_im = (re*c_im + im*c_re) / FIXED_POINT_SCALE
        // Using __int128 for intermediate calculation to prevent overflow before shifting
        __int128 temp_re = (__int128)re * c_re - (__int128)im * c_im;
        __int128 temp_im = (__int128)re * c_im + (__int128)im * c_re;

        q->amplitude.re = (int64_t)(temp_re >> 32); // Assuming FIXED_POINT_SCALE is 2^32
        q->amplitude.im = (int64_t)(temp_im >> 32); // Assuming FIXED_POINT_SCALE is 2^32

        log_symbolic_event("SQRT_X", q->id, q->tag, "Applied √X gate (liminal rotation)");
        return 0;
    }
    // Export the symbol for this function so other kernel modules/code can call it directly.
    EXPORT_SYMBOL_GPL(nymya_3307_sqrt_x_gate);


    /*
     * Kernel syscall: nymya_3307_sqrt_x_gate
     * This is the syscall entry point that wraps the core nymya_3307_sqrt_x_gate function.
     * It handles user-space copy operations before and after calling the core logic.
     */
    SYSCALL_DEFINE1(nymya_3307_sqrt_x_gate, struct nymya_qubit __user *, user_q) {
        struct nymya_qubit kq;
        int ret;

        if (!user_q)
            return -EINVAL;
        if (copy_from_user(&kq, user_q, sizeof(kq)))
            return -EFAULT;

        // Call the core logic function defined above
        ret = nymya_3307_sqrt_x_gate(&kq);

        if (ret) // If the core function returned an error, propagate it
            return ret;

        if (copy_to_user(user_q, &kq, sizeof(kq)))
            return -EFAULT;

        return 0;
    }

#endif

