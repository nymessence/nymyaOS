#ifndef NYMYA_H
#define NYMYA_H

#ifdef __KERNEL__
    #include <linux/types.h>
    #include <linux/string.h>

    typedef struct {
        double re, im;
    } complex_double;

    static inline complex_double make_complex(double re, double im) {
        complex_double c = { re, im };
        return c;
    }

    static inline complex_double complex_mul(complex_double a, complex_double b) {
        complex_double result;
        result.re = a.re * b.re - a.im * b.im;
        result.im = a.re * b.im + a.im * b.re;
        return result;
    }

    // Stubbed in kernel space: trig not available
    static inline complex_double complex_exp_i(double theta) {
        complex_double result = {1.0, 0.0}; // TODO: Replace with kernel-safe trig if needed
        return result;
    }

#else
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <complex.h>
    #include <math.h>

    typedef _Complex double complex_double;

    static inline complex_double make_complex(double re, double im) {
        return re + im * I;
    }

    static inline complex_double complex_mul(complex_double a, complex_double b) {
        return a * b;
    }

    static inline complex_double complex_exp_i(double theta) {
        return cexp(I * theta);
    }
#endif

// Shared structures
typedef struct nymya_qubit {
    uint64_t id;
    const char* tag;
    complex_double amplitude;
} nymya_qubit;

typedef struct {
    double x, y, z;
    nymya_qubit* q;
} nymya_qpos3d;

typedef struct {
    double x, y, z, w;
    nymya_qubit* q;
} nymya_qpos4d;

typedef struct {
    double x, y, z, w, v;
    nymya_qubit* q;
} nymya_qpos5d;

// Shared function declarations
int log_symbolic_event(const char* gate, uint64_t id, const char* tag, const char* msg);

// Quantum-symbolic syscalls
int nymya_3301_identity_gate(nymya_qubit* q);
int nymya_3302_global_phase(nymya_qubit* q, double theta);
int nymya_3303_pauli_x(nymya_qubit* q);
int nymya_3304_pauli_y(nymya_qubit* q);
int nymya_3305_pauli_z(nymya_qubit* q);
int nymya_3306_phase_gate(nymya_qubit* q);
int nymya_3307_sqrt_x_gate(nymya_qubit* q);
int nymya_3308_hadamard_gate(nymya_qubit* q);
int nymya_3309_controlled_not(nymya_qubit* q_ctrl, nymya_qubit* q_target);
int nymya_3310_anticontrol_not(nymya_qubit* q_ctrl, nymya_qubit* q_target);
int nymya_3311_controlled_z(nymya_qubit* q_ctrl, nymya_qubit* q_target);
int nymya_3312_double_controlled_not(nymya_qubit* qc1, nymya_qubit* qc2, nymya_qubit* qt);
int nymya_3313_swap(nymya_qubit* q1, nymya_qubit* q2);
int nymya_3314_imaginary_swap(nymya_qubit* q1, nymya_qubit* q2);
int nymya_3315_phase_shift(nymya_qubit* q, double theta);
int nymya_3316_phase_gate(nymya_qubit* q, double phi);
int nymya_3317_controlled_phase(nymya_qubit* qc, nymya_qubit* qt, double theta);
int nymya_3318_controlled_phase_s(nymya_qubit* qc, nymya_qubit* qt);
int nymya_3319_rotate_x(nymya_qubit* q, double theta);
int nymya_3320_rotate_y(nymya_qubit* q, double theta);
int nymya_3321_rotate_z(nymya_qubit* q, double theta);
int nymya_3322_xx_interaction(nymya_qubit* q1, nymya_qubit* q2, double theta);
int nymya_3323_yy_interaction(nymya_qubit* q1, nymya_qubit* q2, double theta);
int nymya_3324_zz_interaction(nymya_qubit* q1, nymya_qubit* q2, double theta);
int nymya_3325_xyz_entangle(nymya_qubit* q1, nymya_qubit* q2, double theta);
int nymya_3326_sqrt_swap(nymya_qubit* q1, nymya_qubit* q2);
int nymya_3327_sqrt_iswap(nymya_qubit* q1, nymya_qubit* q2);
int nymya_3328_swap_pow(nymya_qubit* q1, nymya_qubit* q2, double alpha);
int nymya_3329_fredkin(nymya_qubit* q_ctrl, nymya_qubit* q1, nymya_qubit* q2);
int nymya_3330_rotate(nymya_qubit* q, char axis, double theta);
int nymya_3331_barenco(nymya_qubit* q1, nymya_qubit* q2, nymya_qubit* q3);
int nymya_3332_berkeley(nymya_qubit* q1, nymya_qubit* q2, double theta);
int nymya_3333_c_v(nymya_qubit* qc, nymya_qubit* qt);
int nymya_3334_core_entangle(nymya_qubit* q1, nymya_qubit* q2);
int nymya_3335_dagwood(nymya_qubit* q1, nymya_qubit* q2, nymya_qubit* q3);
int nymya_3336_echo_cr(nymya_qubit* q1, nymya_qubit* q2, double theta);
int nymya_3337_fermion_sim(nymya_qubit* q1, nymya_qubit* q2);
int nymya_3338_givens(nymya_qubit* q1, nymya_qubit* q2, double theta);
int nymya_3339_magic(nymya_qubit* q1, nymya_qubit* q2);
int nymya_3340_sycamore(nymya_qubit* q1, nymya_qubit* q2);
int nymya_3341_cz_swap(nymya_qubit* q1, nymya_qubit* q2);
int nymya_3342_deutsch(nymya_qubit* q1, nymya_qubit* q2, void (*f)(nymya_qubit*));
int nymya_3343_margolis(nymya_qubit* qc1, nymya_qubit* qc2, nymya_qubit* qt);
int nymya_3344_peres(nymya_qubit* q1, nymya_qubit* q2, nymya_qubit* q3);
int nymya_3345_cf_swap(nymya_qubit* qc, nymya_qubit* q1, nymya_qubit* q2);
int nymya_3346_triangular_lattice(nymya_qubit* q1, nymya_qubit* q2, nymya_qubit* q3);
int nymya_3347_hexagonal_lattice(nymya_qubit* q[6]);
int nymya_3348_hex_rhombi_lattice(nymya_qubit* q[7]);
int nymya_3349_tessellated_triangles(nymya_qubit* q[], size_t count);
int nymya_3350_tessellated_hexagons(nymya_qubit* q[], size_t count);
int nymya_3351_tessellated_hex_rhombi(nymya_qubit* q[], size_t count);
int nymya_3352_e8_group(nymya_qubit* q[8]);
int nymya_3353_flower_of_life(nymya_qubit* q[], size_t count);
int nymya_3354_metatron_cube(nymya_qubit* q[], size_t count);
int nymya_3355_fcc_lattice(nymya_qpos3d qubits[], size_t count);
int nymya_3356_hcp_lattice(nymya_qpos3d qubits[], size_t count);
int nymya_3357_e8_projected_lattice(nymya_qpos3d qubits[], size_t count);
int nymya_3358_d4_lattice(nymya_qpos4d qubits[], size_t count);
int nymya_3359_b5_lattice(nymya_qpos5d qubits[], size_t count);
int nymya_3360_e5_projected_lattice(nymya_qpos5d qubits[], size_t count);
int nymya_3361_qrng_range(uint64_t* out, uint64_t min, uint64_t max, size_t count);

// Shared complex math macros
#define nymya_cexp(theta) complex_exp_i(theta)
#define nymya_cmul(a, b)  complex_mul((a), (b))

// Gate macros (for clean user-facing syntax)
#define identity(q)                    nymya_3301_identity_gate(q)
#define NYMYA_IDENTITY_GATE_CODE       3301

#define global_phase(q, theta)         nymya_3302_global_phase(q, theta)
#define NYMYA_GLOBAL_PHASE_CODE        3302

#define pauli_x(q)                     nymya_3303_pauli_x(q)
#define NYMYA_PAULI_X_CODE             3303

#define pauli_y(q)                     nymya_3304_pauli_y(q)
#define NYMYA_PAULI_Y_CODE             3304

#define pauli_z(q)                     nymya_3305_pauli_z(q)
#define NYMYA_PAULI_Z_CODE             3305

#define phase_s(q)                     nymya_3306_phase_gate(q)
#define NYMYA_PHASE_S_CODE             3306

#define sqrt_x(q)                      nymya_3307_sqrt_x_gate(q)
#define NYMYA_SQRT_X_CODE              3307

#define hadamard(q)                    nymya_3308_hadamard_gate(q)
#define NYMYA_HADAMARD_CODE            3308

#define cnot(qc, qt)                   nymya_3309_controlled_not(qc, qt)
#define NYMYA_CNOT_CODE                3309

#define acnot(qc, qt)                  nymya_3310_anticontrol_not(qc, qt)
#define NYMYA_ACNOT_CODE               3310

#define cz(qc, qt)                     nymya_3311_controlled_z(qc, qt)
#define NYMYA_CZ_CODE                  3311

#define dcnot(q1, q2, qt)              nymya_3312_double_controlled_not(q1, q2, qt)
#define NYMYA_DCNOT_CODE               3312

#define nymya_swap(q1, q2)                   nymya_3313_swap(q1, q2)
#define NYMYA_SWAP_CODE                3313

#define imswap(q1, q2)                 nymya_3314_imaginary_swap(q1, q2)
#define NYMYA_IMSWAP_CODE              3314

#define phase_shift(q, theta)          nymya_3315_phase_shift(q, theta)
#define NYMYA_PHASE_SHIFT_CODE         3315

#define phase_gate(q, phi)             nymya_3316_phase_gate(q, phi)
#define NYMYA_PHASE_GATE_CODE          3316

#define cphase(qc, qt, theta)          nymya_3317_controlled_phase(qc, qt, theta)
#define NYMYA_CPHASE_CODE              3317

#define cphase_s(qc, qt)               nymya_3318_controlled_phase_s(qc, qt)
#define NYMYA_CPHASE_S_CODE            3318

#define rotate_x(q, theta)             nymya_3319_rotate_x(q, theta)
#define NYMYA_ROTATE_X_CODE            3319

#define rotate_y(q, theta)             nymya_3320_rotate_y(q, theta)
#define NYMYA_ROTATE_Y_CODE            3320

#define rotate_z(q, theta)             nymya_3321_rotate_z(q, theta)
#define NYMYA_ROTATE_Z_CODE            3321

#define xx(q1, q2, theta)              nymya_3322_xx_interaction(q1, q2, theta)
#define NYMYA_XX_CODE                  3322

#define yy(q1, q2, theta)              nymya_3323_yy_interaction(q1, q2, theta)
#define NYMYA_YY_CODE                  3323

#define zz(q1, q2, theta)              nymya_3324_zz_interaction(q1, q2, theta)
#define NYMYA_ZZ_CODE                  3324

#define xyz(q1, q2, theta)             nymya_3325_xyz_entangle(q1, q2, theta)
#define NYMYA_XYZ_CODE                 3325

#define sqrt_swap(q1, q2)              nymya_3326_sqrt_swap(q1, q2)
#define NYMYA_SQRT_SWAP_CODE           3326

#define sqrt_iswap(q1, q2)             nymya_3327_sqrt_iswap(q1, q2)
#define NYMYA_SQRT_ISWAP_CODE          3327

#define swap_pow(q1, q2, a)            nymya_3328_swap_pow(q1, q2, a)
#define NYMYA_SWAP_POW_CODE            3328

#define fredkin(qc, q1, q2)            nymya_3329_fredkin(qc, q1, q2)
#define NYMYA_FREDKIN_CODE             3329

#define rotate(q, a, t)                nymya_3330_rotate(q, a, t)
#define NYMYA_ROTATE_CODE              3330

#define barenco(q1, q2, q3)            nymya_3331_barenco(q1, q2, q3)
#define NYMYA_BARENCO_CODE             3331

#define berkeley(q1, q2, t)            nymya_3332_berkeley(q1, q2, t)
#define NYMYA_BERKELEY_CODE            3332

#define c_v(qc, qt)                    nymya_3333_c_v(qc, qt)
#define NYMYA_C_V_CODE                 3333

#define core_entangle(q1, q2)          nymya_3334_core_entangle(q1, q2)
#define NYMYA_CORE_ENTANGLE_CODE       3334

#define dagwood(q1, q2, q3)            nymya_3335_dagwood(q1, q2, q3)
#define NYMYA_DAGWOOD_CODE             3335

#define echo_cr(q1, q2, t)             nymya_3336_echo_cr(q1, q2, t)
#define NYMYA_ECHO_CR_CODE             3336

#define fermion_sim(q1, q2)            nymya_3337_fermion_sim(q1, q2)
#define NYMYA_FERMION_SIM_CODE         3337

#define givens(q1, q2, t)              nymya_3338_givens(q1, q2, t)
#define NYMYA_GIVENS_CODE              3338

#define magic(q1, q2)                  nymya_3339_magic(q1, q2)
#define NYMYA_MAGIC_CODE               3339

#define sycamore(q1, q2)               nymya_3340_sycamore(q1, q2)
#define NYMYA_SYCAMORE_CODE            3340

#define cz_swap(q1, q2)                nymya_3341_cz_swap(q1, q2)
#define NYMYA_CZ_SWAP_CODE             3341

#define deutsch(q1, q2, f)             nymya_3342_deutsch(q1, q2, f)
#define NYMYA_DEUTSCH_CODE             3342

#define margolis(qc1, qc2, qt)         nymya_3343_margolis(qc1, qc2, qt)
#define NYMYA_MARGOLIS_CODE            3343

#define peres(q1, q2, q3)              nymya_3344_peres(q1, q2, q3)
#define NYMYA_PERES_CODE               3344

#define cf_swap(qc, q1, q2)            nymya_3345_cf_swap(qc, q1, q2)
#define NYMYA_CF_SWAP_CODE             3345

#define triangular_lattice(q1, q2, q3) nymya_3346_triangular_lattice(q1, q2, q3)
#define NYMYA_TRIANGULAR_LATTICE_CODE  3346

#define hexagonal_lattice(q)           nymya_3347_hexagonal_lattice(q)
#define NYMYA_HEXAGONAL_LATTICE_CODE   3347

#define hex_rhombi_lattice(q)          nymya_3348_hex_rhombi_lattice(q)
#define NYMYA_HEX_RHOMBI_LATTICE_CODE  3348

#define tessellate_triangles(q, n)     nymya_3349_tessellated_triangles(q, n)
#define NYMYA_TESS_TRIANGLES_CODE      3349

#define tessellate_hexagons(q, n)      nymya_3350_tessellated_hexagons(q, n)
#define NYMYA_TESS_HEXAGONS_CODE       3350

#define tessellate_hex_rhombi(q, n)    nymya_3351_tessellated_hex_rhombi(q, n)
#define NYMYA_TESS_HEX_RHOMBI_CODE     3351

#define e8_group(q)                    nymya_3352_e8_group(q)
#define NYMYA_E8_GROUP_CODE            3352

#define flower_of_life(q, n)           nymya_3353_flower_of_life(q, n)
#define NYMYA_FLOWER_OF_LIFE_CODE      3353

#define metatron_cube(q, n)            nymya_3354_metatron_cube(q, n)
#define NYMYA_METATRON_CUBE_CODE       3354

#define fcc_lattice(q, n)              nymya_3355_fcc_lattice(q, n)
#define NYMYA_FCC_LATTICE_CODE         3355

#define hcp_lattice(q, n)              nymya_3356_hcp_lattice(q, n)
#define NYMYA_HCP_LATTICE_CODE         3356

#define e8_projected_lattice(q, n)     nymya_3357_e8_projected_lattice(q, n)
#define NYMYA_E8_PROJECTED_CODE        3357

#define d4_lattice(q, n)               nymya_3358_d4_lattice(q, n)
#define NYMYA_D4_LATTICE_CODE          3358

#define b5_lattice(q, n)               nymya_3359_b5_lattice(q, n)
#define NYMYA_B5_LATTICE_CODE          3359

#define e5_projected_lattice(q, n)     nymya_3360_e5_projected_lattice(q, n)
#define NYMYA_E5_PROJECTED_CODE        3360

#define qrng_range(out, min, max, count) nymya_3361_qrng_range(out, min, max, count)
#define NYMYA_QRNG_CODE                3361

#endif // NYMYA_H