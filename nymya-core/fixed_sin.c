#include "nymya.h"

/**
 * fixed_sin - Fixed-point sine approximation.
 * @theta: angle in Q32.32 fixed-point format (radians).
 *
 * Approximates sin(theta) using a 7th-order Taylor series expansion around 0:
 *    sin(x) ≈ x − x³/6 + x⁵/120 − x⁷/5040
 *
 * Uses 128-bit intermediate arithmetic to avoid overflow and maintain precision.
 *
 * Note: Best accuracy near zero; inputs outside ±π/2 should be wrapped appropriately.
 *
 * Return:
 *   sine(theta) in Q32.32 fixed-point format.
 */
int64_t fixed_sin(const int64_t theta)
{
    const int64_t ONE = FIXED_POINT_SCALE; // 1.0 in Q32.32

    __int128 x = theta;
    __int128 x2 = (x * x) >> 32;       // x² in Q32.32
    __int128 x3 = (x2 * x) >> 32;      // x³
    __int128 x5 = (x3 * x2) >> 32;     // x⁵
    __int128 x7 = (x5 * x2) >> 32;     // x⁷

    __int128 term1 = x;                // x
    __int128 term2 = x3 / 6;           // x³/6
    __int128 term3 = x5 / 120;         // x⁵/120
    __int128 term4 = x7 / 5040;        // x⁷/5040

    __int128 result = term1 - term2 + term3 - term4;

    return (int64_t)result;
}

