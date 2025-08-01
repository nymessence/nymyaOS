// src/complex_conj.c

#include "nymya.h"

/**
 * complex_conj - Returns the complex conjugate of a complex_double.
 *
 * In kernel mode, complex_double is a struct with fixed-point fields.
 * In user mode, complex_double is _Complex double from <complex.h>.
 *
 * @c: Input complex_double value.
 *
 * Returns:
 * The complex conjugate of the input.
 */
complex_double complex_conj(complex_double c) {
#ifdef __KERNEL__
    complex_double result;
    result.re = c.re;
    result.im = -c.im;
    return result;
#else
    return conj(c);
#endif
}

