// src/fixed_point_sin.c

#include "nymya.h"
#include "nymya_fixed_point.h"

/**
 * fixed_point_sin - Calculate the sine of a fixed-point angle.
 * @angle_fp: The angle in Q32.32 fixed-point format.
 *
 * Normalizes the angle to the range [-PI, PI] and uses a Taylor series
 * expansion to approximate sine: sin(x) ≈ x − x³/6.
 *
 * Returns:
 *   The sine of angle_fp as a Q32.32 fixed-point int64_t.
 */
int64_t fixed_point_sin(int64_t angle_fp) {
    // Normalize to [-PI, PI]
    while (angle_fp > FIXED_POINT_PI)
        angle_fp -= (FIXED_POINT_PI << 1);
    while (angle_fp < -FIXED_POINT_PI)
        angle_fp += (FIXED_POINT_PI << 1);

    // Taylor series: sin(x) ≈ x − x³/6
    int64_t x = angle_fp;
    int64_t x2 = fixed_point_mul(x, x);
    int64_t x3 = fixed_point_mul(x2, x);
    int64_t sixth = (int64_t)((1.0/6.0) * FIXED_POINT_SCALE);
    int64_t term2 = fixed_point_mul(x3, sixth);
    return x - term2;
}
