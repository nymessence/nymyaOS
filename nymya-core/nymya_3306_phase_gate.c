// src/nymya_3306_phase_gate.c

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_nymya_3306_phase_gate NYMYA_PHASE_S_CODE

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
     * nymya_3306_phase_gate - Core kernel function for Phase (S) gate logic.
     * @q: Pointer to the nymya_qubit structure.
     *
     * This function applies the Phase (S) gate logic to a qubit's amplitude.
     * It multiplies the amplitude by fixed-point 'i' (equivalent to phase shift π/2).
     * This function is designed to be called directly by other kernel code.
     *
     * Returns: 0 on success.
     */
    int nymya_3306_phase_gate(struct nymya_qubit *q) {
        int64_t re, im;

        if (!q) {
            pr_err("NYMYA: nymya_3306_phase_gate received NULL qubit pointer\n");
            return -EINVAL; // Return an error if qubit is null
        }

        re = q->amplitude.re;
        im = q->amplitude.im;

        // (a + bi) * i = -b + ai
        q->amplitude.re = -im;
        q->amplitude.im = re;

        log_symbolic_event("PHASE_S", q->id, q->tag, "Applied S gate (π/2 phase)");
        return 0;
    }
    // Export the symbol for this function so other kernel modules/code can call it directly.
    EXPORT_SYMBOL_GPL(nymya_3306_phase_gate);

    /*
     * Kernel syscall: nymya_3306_phase_gate
     * This is the syscall entry point that wraps the core nymya_3306_phase_gate function.
     * It handles user-space copy operations before and after calling the core logic.
     */
    SYSCALL_DEFINE1(nymya_3306_phase_gate, struct nymya_qubit __user *, user_q) {
        struct nymya_qubit kq;
        int ret;

        if (!user_q)
            return -EINVAL;

        // Copy qubit data from user space to kernel space
        if (copy_from_user(&kq, user_q, sizeof(kq)))
            return -EFAULT;

        // Call the core logic function defined above
        ret = nymya_3306_phase_gate(&kq);

        if (ret) // If the core function returned an error, propagate it
            return ret;

        // Copy modified qubit data from kernel space back to user space
        if (copy_to_user(user_q, &kq, sizeof(kq)))
            return -EFAULT;

        return 0;
    }

#endif

