#ifndef NYMYA_BACKEND_GATEQPU_H
#define NYMYA_BACKEND_GATEQPU_H

#include <stdint.h>
#include <nymya/nymya.h>

// Core gate executor for gate-based QPUs (Google Willow, IBM-Q, etc.)
int backend_gateqpu_apply_gate(int gate_code, void* args);

#endif // NYMYA_BACKEND_GATEQPU_H
