// Attempt to detect kernel environment if __KERNEL__ is not defined by build system.
// This is a workaround for build systems that might not consistently define __KERNEL__.
#ifndef __KERNEL__
    #if defined(__linux__) && defined(__GNUC__)
        // Check for common kernel headers that are unlikely to be in userspace.
        // This relies on GCC's __has_include extension.
        #if __has_include(<linux/kernel.h>) || __has_include(<linux/module.h>) || __has_include(<linux/init.h>)
            #define __KERNEL__ 1
        #endif
    #endif
#endif

// Define __maybe_unused if not already defined
#ifndef __maybe_unused
    #define __maybe_unused __attribute__((__unused__))
#endif

#include "nymya.h"

#ifndef __KERNEL__
    #include <math.h>   // for cos(), sin(), cexp() in user mode
    #include <complex.h> // for I in user mode
#else
    // No math.h for kernel. Fixed-point math functions are in nymya.h.
#endif

#ifdef __KERNEL__

// Kernel mode: complex_double is a struct with re, im (fixed-point)

/**
 * nymya_kernel_cexp - Calculates e^(i * theta) for fixed-point complex numbers in kernel mode.
 * @theta_fp: The angle in fixed-point format.
 * @result: Pointer to a complex_double struct to store the result.
 */
static __maybe_unused void nymya_kernel_cexp(int64_t theta_fp, complex_double *result) {
    result->re = fixed_point_cos(theta_fp);
    result->im = fixed_point_sin(theta_fp);
}

/**
 * nymya_kernel_cmul - Multiplies two fixed-point complex_double numbers in kernel mode.
 * @a: Pointer to the first complex_double operand.
 * @b: Pointer to the second complex_double operand.
 * @result: Pointer to a complex_double struct to store the product.
 */
static __maybe_unused void nymya_kernel_cmul(const complex_double *a, const complex_double *b, complex_double *result) {
    result->re = fixed_point_mul(a->re, b->re) - fixed_point_mul(a->im, b->im);
    result->im = fixed_point_mul(a->re, b->im) + fixed_point_mul(a->im, b->re);
}

#else

// User mode: complex_double is _Complex double (native floating-point complex)

/**
 * nymya_kernel_cexp - Calculates e^(i * theta) for native complex doubles in user mode.
 * @theta: The angle in double-precision floating-point.
 *
 * Returns:
 * complex_double representing e^(i * theta).
 */
static complex_double nymya_kernel_cexp(double theta) {
    return cexp(I * theta);
}

/**
 * nymya_kernel_cmul - Multiplies two native complex_double numbers in user mode.
 * @a: The first complex_double operand.
 * @b: The second complex_double operand.
 *
 * Returns:
 * The product (a * b) as a complex_double.
 */
static complex_double nymya_kernel_cmul(complex_double a, complex_double b) {
    return a * b;
}

#endif

