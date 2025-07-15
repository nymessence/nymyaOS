// nymya_runtime.c
#include <stdio.h>
#include <string.h>
#include <nymya/nymya.h>
#include "backend_sim.h"
#include "backend_gateqpu.h"

// Runtime context
typedef enum {
    NYMYA_BACKEND_SIM,
    NYMYA_BACKEND_GATEQPU
} nymya_backend_t;

static nymya_backend_t active_backend = NYMYA_BACKEND_SIM;

void nymya_set_backend(const char* backend_name) {
    if (strcmp(backend_name, "sim") == 0) {
        active_backend = NYMYA_BACKEND_SIM;
        printf("[nymya_runtime] Switched to simulator backend.\n");
    } else if (strcmp(backend_name, "gateqpu") == 0) {
        active_backend = NYMYA_BACKEND_GATEQPU;
        printf("[nymya_runtime] Switched to gate-based QPU backend.\n");
    } else {
        fprintf(stderr, "[nymya_runtime] Unknown backend: %s\n", backend_name);
    }
}

int nymya_apply_gate(int gate_code, void* args) {
    switch (active_backend) {
        case NYMYA_BACKEND_SIM:
            return backend_sim_apply_gate(gate_code, args);
        case NYMYA_BACKEND_GATEQPU:
            return backend_gateqpu_apply_gate(gate_code, args);
        default:
            fprintf(stderr, "[nymya_runtime] Invalid backend selected.\n");
            return -1;
    }
}
