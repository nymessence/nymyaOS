// complex_exp_i.c

#include "nymya.h"

/**
 * complex_exp_i - e^i*theta.
 *
 * @theta: The angle in radians.
 * Returns the complex number representing e^(i*theta).
 */
complex_double complex_exp_i(double theta) {
    return complex_add(complex_from_real(cos(theta)),
                       complex_mul_real(I_val, sin(theta)));
}

