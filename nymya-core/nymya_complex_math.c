// Attempt to detect kernel environment if __KERNEL__ is not defined by build system.
// This is a workaround for build systems that might not consistently define __KERNEL__.
#ifndef __KERNEL__
    #if defined(__linux__) && defined(__GNUC__)
        // Check for common kernel headers that are unlikely to be in userspace.
        // This relies on GCC's __has_include extension.
        #if __has_include(<linux/kernel.h>) || __has_include(<linux/module.h>) || __has_include(<linux/init.h>)
            #define __KERNEL__ 1
            // You might uncomment the line below for debugging purposes to confirm auto-detection
            // #warning "__KERNEL__ was not defined by build system, auto-detecting kernel environment."
        #endif
    #endif
#endif

#include "nymya.h"

#ifndef __KERNEL__
    #include <math.h>   // for cos(), sin(), cexp() in user mode
    #include <complex.h> // for I in user mode
#else
    // No math.h for kernel. Fixed-point math functions are in nymya.h.
    // No explicit includes for kernel-specific math functions here, they should be in nymya.h
#endif

#ifdef __KERNEL__

// Kernel mode: complex_double is a struct with re, im (fixed-point)

/**
 * nymya_kernel_cexp - Calculates e^(i * theta) for fixed-point complex numbers in kernel mode.
 * @theta_fp: The angle in fixed-point format.
 * @result: Pointer to a complex_double struct to store the result.
 *
 * This function computes the complex exponential e^(i * theta) using
 * fixed-point trigonometric functions provided by nymya.h.
 */
static void nymya_kernel_cexp(int64_t theta_fp, complex_double *result) {
    // Use fixed-point trig functions from nymya.h
    result->re = fixed_point_cos(theta_fp);
    result->im = fixed_point_sin(theta_fp);
}

/**
 * nymya_kernel_cmul - Multiplies two fixed-point complex_double numbers in kernel mode.
 * @a: Pointer to the first complex_double operand.
 * @b: Pointer to the second complex_double operand.
 * @result: Pointer to a complex_double struct to store the product.
 *
 * This function performs complex multiplication using fixed-point arithmetic,
 * leveraging the fixed_point_mul function from nymya.h to handle scaling.
 */
static void nymya_kernel_cmul(const complex_double *a, const complex_double *b, complex_double *result) {
    // (a.re + i * a.im) * (b.re + i * b.im) = (a.re*b.re - a.im*b.im) + i * (a.re*b.im + a.im*b.re)
    result->re = fixed_point_mul(a->re, b->re) - fixed_point_mul(a->im, b->im);
    result->im = fixed_point_mul(a->re, b->im) + fixed_point_mul(a->im, b->re);
}

#else

// User mode: complex_double is _Complex double (native floating-point complex)

/**
 * nymya_kernel_cexp - Calculates e^(i * theta) for native complex doubles in user mode.
 * @theta: The angle in double-precision floating-point.
 *
 * This function uses the standard C complex library function cexp() for
 * complex exponential calculation.
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
 * This function performs complex multiplication using native floating-point
 * complex arithmetic.
 *
 * Returns:
 * The product (a * b) as a complex_double.
 */
static complex_double nymya_kernel_cmul(complex_double a, complex_double b) {
    return a * b;
}

#endif

