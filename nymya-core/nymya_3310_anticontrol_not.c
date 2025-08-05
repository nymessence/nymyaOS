// src/nymya_3310_anticontrol_not.c
//
// Implements the Anti-Controlled-NOT (ACNOT) gate for Nymya qubits.
// Flips the target qubit's phase if the control qubit's amplitude magnitude < 0.5.
//
// User-space uses standard complex doubles.
// Kernel-space uses fixed-point math and safe user memory handling.
//

#include "nymya.h"

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_nymya_3310_anticontrol_not NYMYA_ACNOT_CODE

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
#else
    #include <linux/kernel.

