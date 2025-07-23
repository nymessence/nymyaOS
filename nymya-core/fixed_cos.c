#include "nymya.h"

/**
 * fixed_cos - Fixed-point cosine approximation.
 * @theta: angle in Q32.32 fixed-point format (radians).
 *
 * Uses a 6th-order Taylor series:
 *   cos(x) ≈ 1 − x^2/2 + x^4/24 − x^6/720
 *
 * All intermediate results are in Q32.32, using 128‑bit to avoid overflow.
 *
 * Return: cosine(theta) in Q32.32 fixed-point.
 */
int64_t fixed_cos(int64_t theta)
{
    // alias scale
    const int64_t ONE = FIXED_POINT_SCALE;

    // x
    __int128 x = theta;

    // x^2
    __int128 x2 = (x * x) >> 32;
    // x^4 = (x2 * x2) >>32
    __int128 x4 = (x2 * x2) >> 32;
    // x^6 = (x4 * x2) >>32
    __int128 x6 = (x4 * x2) >> 32;

    // Compute terms in Q32.32:
    __int128 term0 = ONE;                       // 1
    __int128 term1 = x2 / 2;                    // x^2/2
    __int128 term2 = x4 / 24;                   // x^4/24
    __int128 term3 = x6 / 720;                  // x^6/720

    // cos ≈ term0 − term1 + term2 − term3
    __int128 result = term0 - term1 + term2 - term3;

    return (int64_t)result;
}
