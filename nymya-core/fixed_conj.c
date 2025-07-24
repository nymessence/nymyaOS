// src/fixed_conj.c

#include "nymya.h"

/**
 * fixed_conj - Returns the conjugate of a fixed-point complex number as a packed int64_t.
 * 
 * The conjugate of a complex number is obtained by negating the imaginary part.
 * 
 * @re: The real part of the fixed-point complex number (int64_t).
 * @im: The imaginary part of the fixed-point complex number (int64_t).
 * 
 * Returns:
 * - A packed int64_t, where the higher 32 bits are the real part and 
 *   the lower 32 bits are the negated imaginary part.
 */
int64_t fixed_conj(int64_t re, int64_t im) {
    // Negate the imaginary part for the conjugate
    int64_t conj_im = -im;

    // Pack real and imaginary parts into a single int64_t (higher 32 bits = real, lower 32 bits = imaginary)
    int64_t conj = (re << 32) | (conj_im & 0xFFFFFFFF); // Mask to ensure it fits into 32 bits

    return conj;
}

