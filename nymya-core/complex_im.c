// complex_im.c

#include "nymya.h"

/**
 * complex_im - Returns the imaginary component of a complex_double.
 *
 * In kernel mode: returns fixed-point int64_t.
 * In userspace: returns floating-point double.
 *
 * @c: The complex number input.
 *
 * Returns:
 * Imaginary component of the input complex number.
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

