// src/complex_mul.c

#include "nymya.h"

#ifdef __KERNEL__

/**
 * complex_mul - Multiply two fixed-point complex numbers.
 * @a: First operand, Q32.32 fixed-point complex_double struct.
 * @b: Second operand, Q32.32 fixed-point complex_double struct.
 *
 * Uses 128-bit intermediates to compute:
 *   (a.re + i·a.im) * (b.re + i·b.im) =
 *   (a.re*b.re - a.im*b.im) + i*(a.re*b.im + a.im*b.re)
 * Then shifts each product right by 32 bits to maintain Q32.32 format.
 *
 * Returns:
 *   The product as a complex_double in Q32.32 fixed-point.
 */
complex_double complex_mul(complex_double a, complex_double b) {
    __int128 re_part = (__int128)a.re * b.re - (__int128)a.im * b.im;
    __int128 im_part = (__int128)a.re * b.im + (__int128)a.im * b.re;

    complex_double result;
    result.re = (int64_t)(re_part >> 32);
    result.im = (int64_t)(im_part >> 32);
    return result;
}

#else

#include <complex.h>

/**
 * complex_mul - Multiply two native complex_double numbers in userland.
 * @a: First operand, native _Complex double.
 * @b: Second operand, native _Complex double.
 *
 * Returns:
 *   The product (a * b) as native complex_double.
 */
complex_double complex_mul(complex_double a, complex_double b) {
    return a * b;
}

#endif

