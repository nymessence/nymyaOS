#ifndef NYMYA_RUNTIME_H
#define NYMYA_RUNTIME_H

#include <stdint.h>
#include "nymya/nymya.h"

// Set backend: "sim" or "gateqpu"
void nymya_set_backend(const char* backend_name);

// Unified gate execution entry point
int nymya_apply_gate(int gate_code, void* args);

#endif // NYMYA_RUNTIME_H
