#include "nymya.h"
#include <math.h>  // for cos(), sin() in user mode

#ifdef __KERNEL__

// Kernel mode: complex_double is a struct with re, im

static void nymya_kernel_cexp(double theta, complex_double *result) {
    // Kernel mode doesn't have trig by default; 
    // if you have trig support, use it here, otherwise stub
    // Replace with proper kernel trig if available
    result->re = cos(theta);
    result->im = sin(theta);
}

static void nymya_kernel_cmul(const complex_double *a, const complex_double *b, complex_double *result) {
    result->re = a->re * b->re - a->im * b->im;
    result->im = a->re * b->im + a->im * b->re;
}

#else

// User mode: complex_double is _Complex double

static complex_double nymya_kernel_cexp(double theta) {
    return cexp(I * theta);
}

static complex_double nymya_kernel_cmul(complex_double a, complex_double b) {
    return a * b;
}

#endif