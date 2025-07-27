// src/complex_re.c

#include "nymya.h"

#ifdef __KERNEL__

/**
 * complex_re - Extract the real part of a fixed-point complex number.
 * @c: complex_double in Q32.32 fixed-point format.
 *
 * Returns:
 *   The real component as an int64_t in Q32.32.
 */
int64_t complex_re(complex_double c) {
    return c.re;
}

#else

#include <complex.h>

/**
 * complex_re - Extract the real part of a native complex double number.
 * @c: complex_double (native _Complex double).
 *
 * Returns:
 *   The real component as a double.
 */
double complex_re(complex_double c) {
    return creal(c);
}

#endif

