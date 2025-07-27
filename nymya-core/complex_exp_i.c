// src/complex_im.c

#include "nymya.h"
#ifndef __KERNEL__
#include <complex.h>
#endif

/**
 * complex_im - Returns the imaginary part of a complex_double.
 *
 * In kernel mode, complex_double is a struct with a fixed-point `im` field.
 * In user mode, complex_double is a native `_Complex double`.
 *
 * @c: The complex_double input.
 *
 * Returns:
 *   - Kernel: `int64_t` fixed-point imaginary component.
 *   - User: `double` floating-point imaginary component.
 */
#ifdef __KERNEL__
int64_t complex_im(complex_double c) {
    return c.im;
}
#else
double complex_im(complex_double c) {
    return cimag(c);
}
#endif

