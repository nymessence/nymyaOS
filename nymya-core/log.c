// src/log.c

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "nymya.h"

int log_symbolic_event(const char* gate, uint64_t id, const char* tag, const char* msg) {
    printf("[NYMYA %s] Qubit ID %lu (%s): %s\n", gate, id, tag ? tag : "untagged", msg);
    return 0;
}
