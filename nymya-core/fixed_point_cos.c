// src/fixed_point_cos.c

#include "nymya.h"
int64_t fixed_point_mul(int64_t a, int64_t b);

/**
 * fixed_point_cos - Calculate the cosine of a fixed-point angle.
 * @angle_fp: The angle in Q32.32 fixed-point format.
 *
 * Normalizes the angle to the range [-PI, PI] and uses a Taylor series
 * expansion to approximate cosine: cos(x) ≈ 1 − x²/2.
 *
 * Returns:
 *   The cosine of angle_fp as a Q32.32 fixed-point int64_t.
 */
int64_t fixed_point_cos(int64_t angle_fp) {
    // Normalize to [-PI, PI]
    while (angle_fp > FIXED_POINT_PI)
        angle_fp -= (FIXED_POINT_PI << 1);
    while (angle_fp < -FIXED_POINT_PI)
        angle_fp += (FIXED_POINT_PI << 1);

    // Taylor series: cos(x) ≈ 1 − x²/2
    int64_t one = FIXED_POINT_SCALE;
    int64_t x2 = fixed_point_mul(angle_fp, angle_fp);
    int64_t half = (int64_t)(0.5 * FIXED_POINT_SCALE);
    int64_t term2 = fixed_point_mul(x2, half);
    return one - term2;
}
