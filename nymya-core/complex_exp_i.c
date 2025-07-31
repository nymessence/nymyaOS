// complex_exp_i.c

#include "nymya.h"

/**
 * complex_exp_i - Computes e^(i*theta) for fixed-point numbers.
 *
 * @theta: The angle in radians, as a Q32.32 fixed-point value.
 * Returns the complex number representing e^(i*theta).
 */
complex_double complex_exp_i(double theta) {
    return make_complex(cos(theta), sin(theta));
}

