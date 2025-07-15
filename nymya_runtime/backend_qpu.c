// runtime/backend_gateqpu.c

#include <stdio.h>
#include <stdint.h>
#include <nymya/nymya.h>
#include "backend_gateqpu.h"

// Argument structs (same as sim backend)
typedef struct { nymya_qubit* q; } qpu_arg_q;
typedef struct { nymya_qubit* q; double theta; } qpu_arg_q_theta;
typedef struct { nymya_qubit* q1, *q2; } qpu_arg_q2;
typedef struct { nymya_qubit* q1, *q2; double theta; } qpu_arg_q2_theta;
typedef struct { nymya_qubit* q1, *q2, *q3; } qpu_arg_q3;
typedef struct { nymya_qubit** qs; size_t count; } qpu_arg_q_arr;
typedef struct { nymya_qpos3d* qs; size_t count; } qpu_arg_q3d;
typedef struct { nymya_qpos4d* qs; size_t count; } qpu_arg_q4d;
typedef struct { nymya_qpos5d* qs; size_t count; } qpu_arg_q5d;
typedef struct { uint64_t* out; uint64_t min, max; size_t count; } qpu_arg_qrng;

int backend_gateqpu_apply_gate(int gate_code, void* args) {
    switch (gate_code) {
        // Single‐qubit gates
        case 3301: { // identity
            qpu_arg_q* a = args;
            printf("[QPU] ID gate on qubit %ju\n", a->q->id);
            break;
        }
        case 3302: { // global_phase
            qpu_arg_q_theta* a = args;
            printf("[QPU] Global phase %f on qubit %ju\n", a->theta, a->q->id);
            break;
        }
        case 3303: { qpu_arg_q* a = args; printf("[QPU] Pauli-X on %ju\n", a->q->id); break; }
        case 3304: { qpu_arg_q* a = args; printf("[QPU] Pauli-Y on %ju\n", a->q->id); break; }
        case 3305: { qpu_arg_q* a = args; printf("[QPU] Pauli-Z on %ju\n", a->q->id); break; }
        case 3306: { qpu_arg_q* a = args; printf("[QPU] S-gate on %ju\n", a->q->id); break; }
        case 3307: { qpu_arg_q* a = args; printf("[QPU] √X on %ju\n", a->q->id); break; }
        case 3308: { qpu_arg_q* a = args; printf("[QPU] Hadamard on %ju\n", a->q->id); break; }
        case 3315: { qpu_arg_q_theta* a = args; printf("[QPU] Phase shift %f on %ju\n", a->theta, a->q->id); break; }
        case 3316: { qpu_arg_q_theta* a = args; printf("[QPU] Parametric phase %f on %ju\n", a->theta, a->q->id); break; }
        case 3319: { qpu_arg_q_theta* a = args; printf("[QPU] RX(%f) on %ju\n", a->theta, a->q->id); break; }
        case 3320: { qpu_arg_q_theta* a = args; printf("[QPU] RY(%f) on %ju\n", a->theta, a->q->id); break; }
        case 3321: { qpu_arg_q_theta* a = args; printf("[QPU] RZ(%f) on %ju\n", a->theta, a->q->id); break; }

        // Two‐qubit gates
        case 3309: { qpu_arg_q2* a = args; printf("[QPU] CNOT ctrl=%ju tgt=%ju\n", a->q1->id, a->q2->id); break; }
        case 3310: { qpu_arg_q2* a = args; printf("[QPU] ACNOT ctrl=%ju tgt=%ju\n", a->q1->id, a->q2->id); break; }
        case 3311: { qpu_arg_q2* a = args; printf("[QPU] CZ ctrl=%ju tgt=%ju\n", a->q1->id, a->q2->id); break; }
        case 3312: { qpu_arg_q3* a = args; printf("[QPU] Toffoli c1=%ju c2=%ju tgt=%ju\n", a->q1->id, a->q2->id, a->q3->id); break; }
        case 3313: { qpu_arg_q2* a = args; printf("[QPU] SWAP %ju ↔ %ju\n", a->q1->id, a->q2->id); break; }
        case 3314: { qpu_arg_q2* a = args; printf("[QPU] iSWAP %ju ↔ %ju\n", a->q1->id, a->q2->id); break; }
        case 3322: { qpu_arg_q2_theta* a = args; printf("[QPU] XX(%f) on %ju,%ju\n", a->theta, a->q1->id, a->q2->id); break; }
        case 3323: { qpu_arg_q2_theta* a = args; printf("[QPU] YY(%f) on %ju,%ju\n", a->theta, a->q1->id, a->q2->id); break; }
        case 3324: { qpu_arg_q2_theta* a = args; printf("[QPU] ZZ(%f) on %ju,%ju\n", a->theta, a->q1->id, a->q2->id); break; }
        case 3325: { qpu_arg_q2_theta* a = args; printf("[QPU] XYZ-entangle(%f) on %ju,%ju\n", a->theta, a->q1->id, a->q2->id); break; }
        case 3326: { qpu_arg_q2* a = args; printf("[QPU] √SWAP %ju ↔ %ju\n", a->q1->id, a->q2->id); break; }
        case 3327: { qpu_arg_q2* a = args; printf("[QPU] √iSWAP %ju ↔ %ju\n", a->q1->id, a->q2->id); break; }
        case 3328: { qpu_arg_q2_theta* a = args; printf("[QPU] SWAP^α(%f) %ju↔%ju\n", a->theta, a->q1->id, a->q2->id); break; }
        case 3329: { qpu_arg_q3* a = args; printf("[QPU] Fredkin ctrl=%ju a=%ju b=%ju\n", a->q1->id, a->q2->id, a->q3->id); break; }
        case 3333: { qpu_arg_q2* a = args; printf("[QPU] Controlled-V %ju->%ju\n", a->q1->id, a->q2->id); break; }
        case 3334: { qpu_arg_q2* a = args; printf("[QPU] Core entangle %ju,%ju\n", a->q1->id, a->q2->id); break; }
        case 3336: { qpu_arg_q2_theta* a = args; printf("[QPU] Echoed CR(%f) %ju,%ju\n", a->theta, a->q1->id, a->q2->id); break; }
        case 3337: { qpu_arg_q2* a = args; printf("[QPU] Fermion-sim %ju,%ju\n", a->q1->id, a->q2->id); break; }
        case 3338: { qpu_arg_q2_theta* a = args; printf("[QPU] Givens(%f) %ju,%ju\n", a->theta, a->q1->id, a->q2->id); break; }
        case 3339: { qpu_arg_q2* a = args; printf("[QPU] Magic entangle %ju,%ju\n", a->q1->id, a->q2->id); break; }
        case 3340: { qpu_arg_q2* a = args; printf("[QPU] Sycamore %ju,%ju\n", a->q1->id, a->q2->id); break; }
        case 3341: { qpu_arg_q2* a = args; printf("[QPU] CZ+SWAP %ju,%ju\n", a->q1->id, a->q2->id); break; }

        // Three-qubit gates
        case 3330: { qpu_arg_q3* a = args; printf("[QPU] Rotate '%c' %f on %ju\n", *((char*)&a->q1), a->q2->amplitude, a->q1->id); break; }
        case 3331: { qpu_arg_q3* a = args; printf("[QPU] Barenco %ju,%ju,%ju\n", a->q1->id, a->q2->id, a->q3->id); break; }
        case 3332: { qpu_arg_q2_theta* a = args; printf("[QPU] Berkeley(%f) %ju,%ju\n", a->theta, a->q1->id, a->q2->id); break; }
        case 3335: { qpu_arg_q3* a = args; printf("[QPU] Dagwood %ju,%ju,%ju\n", a->q1->id, a->q2->id, a->q3->id); break; }
        case 3336: /* also two-qubit, covered */ break;
        case 3337: /* two-qubit, covered */ break;
        case 3338: /* two-qubit, covered */ break;
        case 3339: /* two-qubit, covered */ break;
        case 3342: { qpu_arg_q3* a = args; printf("[QPU] Deutsch oracle %ju,%ju\n", a->q1->id, a->q2->id); break; }
        case 3343: { qpu_arg_q3* a = args; printf("[QPU] Margolis %ju,%ju->%ju\n", a->q1->id, a->q2->id, a->q3->id); break; }
        case 3344: { qpu_arg_q3* a = args; printf("[QPU] Peres %ju,%ju,%ju\n", a->q1->id, a->q2->id, a->q3->id); break; }
        case 3345: { qpu_arg_q3* a = args; printf("[QPU] CF-SWAP %ju ctrl %ju,%ju\n", a->q1->id, a->q2->id, a->q3->id); break; }

        // Lattice & tessellation gates (arrays)
        case 3346: { qpu_arg_q3* a = args; printf("[QPU] Triangle lattice (%ju,%ju,%ju)\n",
                     a->q1->id, a->q2->id, a->q3->id); break; }
        case 3347: { qpu_arg_q_arr* a = args; printf("[QPU] Hex lattice of %zu qubits\n", a->count); break; }
        case 3348: { qpu_arg_q_arr* a = args; printf("[QPU] Hex→3 rhombi of %zu qubits\n", a->count); break; }
        case 3349: { qpu_arg_q_arr* a = args; printf("[QPU] Tessellate triangles count=%zu\n", a->count); break; }
        case 3350: { qpu_arg_q_arr* a = args; printf("[QPU] Tessellate hexagons count=%zu\n", a->count); break; }
        case 3351: { qpu_arg_q_arr* a = args; printf("[QPU] Tessellate hex rhombi count=%zu\n", a->count); break; }
        case 3352: { qpu_arg_q_arr* a = args; printf("[QPU] E8 group on 8 qubits\n"); break; }
        case 3353: { qpu_arg_q_arr* a = args; printf("[QPU] Flower of Life count=%zu\n", a->count); break; }
        case 3354: { qpu_arg_q_arr* a = args; printf("[QPU] Metatron’s Cube count=%zu\n", a->count); break; }
        case 3355: { qpu_arg_q3d* a = args; printf("[QPU] FCC lattice count=%zu\n", a->count); break; }
        case 3356: { qpu_arg_q3d* a = args; printf("[QPU] HCP lattice count=%zu\n", a->count); break; }
        case 3357: { qpu_arg_q3d* a = args; printf("[QPU] E8 projected lattice count=%zu\n", a->count); break; }
        case 3358: { qpu_arg_q4d* a = args; printf("[QPU] D4 4D lattice count=%zu\n", a->count); break; }
        case 3359: { qpu_arg_q5d* a = args; printf("[QPU] B5 5D lattice count=%zu\n", a->count); break; }
        case 3360: { qpu_arg_q5d* a = args; printf("[QPU] E5 projected 5D lattice count=%zu\n", a->count); break; }

        // Quantum RNG
        case 3361: { 
            qpu_arg_qrng* a = args;
            printf("[QPU] Request QRNG %zu values in [%ju..%ju]\n", a->count, a->min, a->max);
            break;
        }

        default:
            printf("[QPU] Unknown gate code %d\n", gate_code);
            return -1;
    }n

    return 0;
}
