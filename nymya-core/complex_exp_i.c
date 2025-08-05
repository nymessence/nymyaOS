// complex_exp_i.c

#include "nymya.h" // Assumed to define complex_double, fixed_cos, fixed_sin, etc.

#ifdef __KERNEL__
#include <linux/kernel.h> // For pr_info, if used
#include <linux/types.h>  // For int64_t, if not already in nymya.h
// No need for <math.h> here as we are using fixed-point math functions

/**
 * complex_exp_i - Computes e^(i*theta) for fixed-point numbers in kernel-space.
 *
 * @theta_fp: The angle in radians, as a Q32.32 fixed-point value.
 * Returns the complex number representing e^(i*theta).
 */
complex_double complex_exp_i(int64_t theta_fp) { // Corrected parameter type and name
    complex_double result;
    result.re = fixed_cos(theta_fp); // Call fixed-point cosine function
    result.im = fixed_sin(theta_fp); // Call fixed-point sine function
    return result;
}

// If this function needs to be exported for other kernel modules, add EXPORT_SYMBOL_GPL
// EXPORT_SYMBOL_GPL(complex_exp_i); // Uncomment if needed

#else // Userland implementation

#include <complex.h> // For native _Complex double and I macro
#include <math.h>    // For cos, sin
#include <stdio.h>   // For printf, if used for debugging

/**
 * complex_exp_i - Computes e^(i*theta) for native double-precision complex numbers in userland.
 *
 * @theta: The angle in radians, as a double-precision floating-point value.
 * Returns the complex number representing e^(i*theta).
 */
complex_double complex_exp_i(double theta) {
    // In userland, complex_double is typically defined as _Complex double when <complex.h> is included.
    // The previous error "request for member 're' in something not a structure or union"
    // indicates that 'result' was being treated as _Complex double, which doesn't have .re/.im members.
    // The correct way to construct a complex number from real and imaginary parts using <complex.h>
    // is with 'creal' and 'cimag' or directly using 'cos(theta) + I * sin(theta)'.
    return cos(theta) + I * sin(theta);
}

#endif