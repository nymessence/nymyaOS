#include "nymya.h" // Include the header file for definitions like complex_double and FIXED_POINT_SCALE

// This file contains the kernel-specific implementation of fixed-point complex multiplication.
// It must be compiled with the __KERNEL__ macro defined to correctly use complex_double.

#ifdef __KERNEL__

/**
 * fixed_complex_multiply - Multiplies two fixed-point complex numbers (internal static function).
 * @re1: Real part of the first complex number in Q32.32 fixed-point.
 * @im1: Imaginary part of the first complex number in Q32.32 fixed-point.
 * @re2: Real part of the second complex number in Q32.32 fixed-point.
 * @im2: Imaginary part of the second complex number in Q32.32 fixed-point.
 *
 * Multiplies two complex numbers (re1 + im1 * i) and (re2 + im2 * i)
 * using the fixed-point Q32.32 format. The result is also in Q32.32 format.
 *
 * Formula:
 * (re1 + im1 * i) * (re2 + im2 * i) = (re1 * re2 - im1 * im2) + (re1 * im2 + im1 * re2) * i
 *
 * Uses 128-bit intermediate types (`__int128`) to prevent overflow during intermediate
 * multiplications before shifting back to the 64-bit fixed-point format.
 *
 * This function is now 'static', meaning it is only visible and callable within this
 * compilation unit (nymya_complex_math.c). Other files should use `complex_mul`
 * from `nymya.h` which operates on `complex_double` structs.
 *
 * Returns:
 * A `complex_double` struct containing both the real and imaginary parts of the product.
 */
complex_double fixed_complex_multiply(int64_t re1, int64_t im1, int64_t re2, int64_t im2)
{
    // Calculate the real part of the product: (re1 * re2) - (im1 * im2)
    // Use __int128 for intermediate calculations to prevent overflow.
    __int128 re_part = (__int128)re1 * re2 - (__int128)im1 * im2;

    // Calculate the imaginary part of the product: (re1 * im2) + (im1 * re2)
    // Use __int128 for intermediate calculations to prevent overflow.
    __int128 im_part = (__int128)re1 * im2 + (__int128)im1 * re2;

    complex_double result; // Declare result as complex_double

    // Shift right by 32 bits (equivalent to dividing by FIXED_POINT_SCALE)
    // to bring the result back to the Q32.32 fixed-point format.
    result.re = (int64_t)(re_part >> 32);
    result.im = (int64_t)(im_part >> 32);

    return result; // Return the complex_double struct containing both parts
}

#endif // __KERNEL__