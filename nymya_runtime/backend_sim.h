#ifndef NYMYA_BACKEND_SIM_H
#define NYMYA_BACKEND_SIM_H

#include <stdint.h>
#include <nymya/nymya.h>

// Core gate executor for simulation backend
int backend_sim_apply_gate(int gate_code, void* args);

#endif // NYMYA_BACKEND_SIM_H
