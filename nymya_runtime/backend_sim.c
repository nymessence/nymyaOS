// backend_sim.c

#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <stdint.h>
#include <nymya/nymya.h>

// Argument structs
typedef struct { nymya_qubit* q; } sim_arg_q;
typedef struct { nymya_qubit* q; double theta; } sim_arg_q_theta;
typedef struct { nymya_qubit* q1, *q2; } sim_arg_q2;
typedef struct { nymya_qubit* q1, *q2; double theta; } sim_arg_q2_theta;
typedef struct { nymya_qubit* q1, *q2, *q3; } sim_arg_q3;
typedef struct { nymya_qubit** qs; size_t count; } sim_arg_q_arr;
typedef struct { nymya_qpos3d* qs; size_t count; } sim_arg_q3d;
typedef struct { nymya_qpos4d* qs; size_t count; } sim_arg_q4d;
typedef struct { nymya_qpos5d* qs; size_t count; } sim_arg_q5d;
typedef struct { uint64_t* out; uint64_t min, max; size_t count; } sim_arg_qrng;

int backend_sim_apply_gate(int gate_code, void* args) {
    switch (gate_code) {
        // Single-qubit gates
        case 3301: { // identity
            return 0;
        }
        case 3302: { // global_phase
            sim_arg_q_theta* a = args;
            a->q->amplitude *= cexp(I * a->theta);
            return 0;
        }
        case 3303: { // pauli_x
            sim_arg_q* a = args;
            a->q->amplitude = 1.0 - a->q->amplitude;
            return 0;
        }
        case 3304: { // pauli_y
            sim_arg_q* a = args;
            a->q->amplitude = I * (1.0 - a->q->amplitude);
            return 0;
        }
        case 3305: { // pauli_z
            sim_arg_q* a = args;
            a->q->amplitude = -a->q->amplitude;
            return 0;
        }
        case 3306: { // phase_s = π/2
            sim_arg_q* a = args;
            a->q->amplitude *= cexp(I * M_PI_2);
            return 0;
        }
        case 3307: { // sqrt_x
            sim_arg_q* a = args;
            a->q->amplitude = (a->q->amplitude + I * (1.0 - a->q->amplitude)) / M_SQRT2;
            return 0;
        }
        case 3308: { // hadamard
            sim_arg_q* a = args;
            a->q->amplitude = (a->q->amplitude + (1.0 - a->q->amplitude)) / M_SQRT2;
            return 0;
        }
        case 3315: { // phase_shift
            sim_arg_q_theta* a = args;
            a->q->amplitude *= cexp(I * a->theta);
            return 0;
        }
        case 3316: { // phase_gate (φ)
            sim_arg_q_theta* a = args;
            a->q->amplitude *= cexp(I * a->theta);
            return 0;
        }
        case 3319: { // rotate_x
            sim_arg_q_theta* a = args;
            a->q->amplitude *= cos(a->theta/2) + I * sin(a->theta/2);
            return 0;
        }
        case 3320: { // rotate_y
            sim_arg_q_theta* a = args;
            a->q->amplitude *= cos(a->theta/2) + sin(a->theta/2);
            return 0;
        }
        case 3321: { // rotate_z
            sim_arg_q_theta* a = args;
            a->q->amplitude *= cexp(I * a->theta);
            return 0;
        }

        // Two-qubit gates
        case 3309: // cnot
        case 3310: // acnot
        case 3311: // controlled_z
        case 3312: // double_controlled_not
        case 3313: // swap
        case 3314: // imaginary_swap
        case 3322: // xx_interaction
        case 3323: // yy_interaction
        case 3324: // zz_interaction
        case 3325: // xyz_entangle
        case 3326: // sqrt_swap
        case 3327: // sqrt_iswap
        case 3328: // swap_pow
        case 3329: // fredkin
        case 3333: // c_v
        case 3334: // core_entangle
        case 3336: // echo_cr
        case 3337: // fermion_sim
        case 3338: // givens
        case 3339: // magic
        case 3340: // sycamore
        case 3341: // cz_swap
            printf("[sim backend] Two-qubit (or interaction) gate %d stub\n", gate_code);
            return 0;

        // Three-qubit gates
        case 3330: // generic rotate (reinterpret as single)
        case 3331: // barenco
        case 3332: // berkeley
        case 3335: // dagwood
        case 3342: // deutsch
        case 3343: // margolis
        case 3344: // peres
        case 3345: // cf_swap
            printf("[sim backend] Three-qubit gate %d stub\n", gate_code);
            return 0;

        // Lattice & tessellation gates (arrays)
        case 3346: // triangular_lattice
        case 3347: // hexagonal_lattice
        case 3348: // hex_rhombi_lattice
        case 3349: // tessellated_triangles
        case 3350: // tessellated_hexagons
        case 3351: // tessellated_hex_rhombi
        case 3352: // e8_group
        case 3353: // flower_of_life
        case 3354: // metatron_cube
        case 3355: // fcc_lattice
        case 3356: // hcp_lattice
        case 3357: // e8_projected_lattice
        case 3358: // d4_lattice
        case 3359: // b5_lattice
        case 3360: // e5_projected_lattice
            printf("[sim backend] Lattice gate %d stub\n", gate_code);
            return 0;

        // Quantum RNG
        case 3361: { // qrng_range
            sim_arg_qrng* a = args;
            for (size_t i = 0; i < a->count; i++) {
                // simple pseudo-random fallback
                a->out[i] = a->min + (rand() % (a->max - a->min + 1));
            }
            return 0;
        }

        default:
            printf("[sim backend] Unknown gate code %d\n", gate_code);
            return -1;
    }
}
