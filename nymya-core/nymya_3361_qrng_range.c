// src/nymya_3361_qrng_range.c

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "nymya.h"

#ifndef __KERNEL__
    #include <stdio.h>
#else
    #include <linux/kernel.h>
    #include <linux/syscalls.h>
    #include <linux/uaccess.h>
#endif

#ifndef __KERNEL__

int nymya_3361_qrng_range(uint64_t* out, uint64_t min, uint64_t max, size_t count) {
    if (!out || min >= max || count == 0) return -1;

    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    uint64_t range = max - min + 1;
    uint64_t half_range = range / 2;

    for (size_t i = 0; i < count; i++) {
        nymya_qubit q = {
            .id = i,
            .tag = "qrng",
            .amplitude = 1.0 + 0.0 * I
        };

        hadamard(&q);           // Symbolic superposition
        global_phase(&q, 0.0);  // Identity phase, symbolic

        int bit = (rand() % 2);

        out[i] = min + bit * half_range;

        log_symbolic_event("QRNG_BIT", q.id, q.tag, bit ? "1" : "0");
    }

    return 0;
}

#else

SYSCALL_DEFINE4(nymya_3361_qrng_range,
    uint64_t __user *, user_out,
    uint64_t, min,
    uint64_t, max,
    size_t, count) {

    if (!user_out || min >= max || count == 0)
        return -EINVAL;

    uint64_t* k_out = kmalloc(sizeof(uint64_t) * count, GFP_KERNEL);
    if (!k_out)
        return -ENOMEM;

    static int seeded = 0;
    if (!seeded) {
        // Kernel random seed (for demo, use get_random_bytes for better RNG)
        srand((unsigned int)ktime_get_real_seconds());
        seeded = 1;
    }

    uint64_t range = max - min + 1;
    uint64_t half_range = range / 2;

    for (size_t i = 0; i < count; i++) {
        struct nymya_qubit q = {
            .id = i,
            .tag = "qrng",
            .amplitude = 1.0 + 0.0 * I
        };

        nymya_hadamard(&q);       // Assuming kernel-space equivalent exists
        nymya_global_phase(&q, 0.0);

        int bit = prandom_u32() & 1;  // Better kernel RNG than rand()

        k_out[i] = min + bit * half_range;

        // Log event - assuming a kernel-safe version
        log_symbolic_event("QRNG_BIT", q.id, q.tag, bit ? "1" : "0");
    }

    if (copy_to_user(user_out, k_out, sizeof(uint64_t) * count)) {
        kfree(k_out);
        return -EFAULT;
    }

    kfree(k_out);
    return 0;
}

#endif
