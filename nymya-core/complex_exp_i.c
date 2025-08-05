// complex_exp_i.c

#include "nymya.h"

#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/types.h>

/**
 * complex_exp_i - Computes e^(i*theta) for fixed-point numbers in kernel-space.
 *
 * @theta_fp: The angle in radians, as a Q32.32 fixed-point value.
 * Returns the complex number representing e^(i*theta).
 */
complex_double complex_exp_i(int64_t theta_fp) {
    complex_double result;
    result.re = fixed_cos(theta_fp);
    result.im = fixed_sin(theta_fp);
    return result;
}

// EXPORT_SYMBOL_GPL(complex_exp_i);

#else // Userland implementation

#include <complex.h>
#include <math.h>
#include <stdio.h>

/**
 * complex_exp_i - Computes e^(i*theta) for native double-precision complex numbers in userland.
 *
 * @theta: The angle in radians, as a double-precision floating-point value.
 * Returns the complex number representing e^(i*theta).
 */
complex_double complex_exp_i(double theta) {
    return cos(theta) + I * sin(theta);
}

#endif