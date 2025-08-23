#ifndef NYMYA_H
#define NYMYA_H

// Fixed-point scale for Q32.32 format.
// 1 unit = 2^-32 in real value.
#define FIXED_POINT_SCALE (1ULL << 32)

// Maximum length for qubit tags (labels)
#define NYMYA_TAG_MAXLEN 32

// Fixed-point constants for kernel calculations
#define FIXED_POINT_PI (int64_t)(3.141592653589793 * FIXED_POINT_SCALE)
#define FIXED_POINT_PI_DIV_2 (int64_t)(1.5707963267948966 * FIXED_POINT_SCALE)
#define FIXED_POINT_SQRT2_INV_FP (int64_t)(0.7071067811865476 * FIXED_POINT_SCALE)

#ifdef __KERNEL__

    #include <linux/types.h>
    #include <linux/string.h>

    typedef __s64 int64_t;
    typedef __u64 uint64_t;

    /**
     * complex_double - Fixed-point complex number type for kernel space.
     */
    typedef struct {
        int64_t re;
        int64_t im;
    } complex_double;

    /* Fixed-point math function prototypes */
    int64_t fixed_point_mul(int64_t val1, int64_t val2);
    int64_t fixed_point_cos(int64_t angle_fp);
    int64_t fixed_point_sin(int64_t angle_fp);
    int64_t fixed_point_square(int64_t val);
    complex_double make_complex(int64_t re_fp, int64_t im_fp);
    complex_double complex_mul(complex_double a, complex_double b);
    complex_double complex_exp_i(int64_t theta_fp);
    int64_t complex_re(complex_double c);
    int64_t complex_im(complex_double c);
    complex_double complex_conj(complex_double c);
    int64_t fixed_sin(int64_t theta);
    int64_t fixed_cos(int64_t theta);
    complex_double complex_exp_i(int64_t theta_fp);
    
    // additional functions
    int64_t fixed_sin(int64_t theta);
    int64_t fixed_cos(int64_t theta);
    int64_t fixed_conj(int64_t re, int64_t im);

#else // userspace

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <complex.h>
    #include <string.h>
    #include <math.h>

    /**
     * complex_double - Userspace native complex double type.
     */
    typedef _Complex double complex_double;

    /* Userspace complex math function prototypes */
    complex_double make_complex(double re, double im);
    complex_double complex_mul(complex_double a, complex_double b);
    complex_double complex_exp_i(double theta);
    double complex_re(complex_double c);
    double complex_im(complex_double c);
    complex_double complex_conj(complex_double c);

#endif // __KERNEL__


// Common structure definitions, visible to both kernel and userspace
/**
 * nymya_qubit - Qubit struct.
 * @id: Unique qubit identifier.
 * @tag: Label/tag for qubit, max NYMYA_TAG_MAXLEN chars.
 * @amplitude: Qubit amplitude as complex number (type depends on compilation).
 */
typedef struct nymya_qubit {
    uint64_t id;
    char tag[NYMYA_TAG_MAXLEN];
    complex_double amplitude; // This type is conditionally defined above
} nymya_qubit;

/**
 * nymya_qpos3d_k - 3D fixed-point position struct for kernel space (and userland marshalling).
 * @q: Associated qubit.
 * @x, y, z: Q32.32 fixed-point coordinates.
 */
typedef struct {
    nymya_qubit q;
    int64_t    x, y, z;
} nymya_qpos3d_k;

/**
 * nymya_qpos4d_k - 4D fixed-point position struct for kernel space (and userland marshalling).
 * @q: Associated qubit.
 * @x, y, z, w: Q32.32 fixed-point coordinates.
 */
typedef struct {
    nymya_qubit q;
    int64_t    x, y, z, w;
} nymya_qpos4d_k;

/**
 * nymya_qpos5d_k - 5D fixed-point position struct for kernel space (and userland marshalling).
 * @q: Associated qubit.
 * @x, y, z, w, v: Q32.32 fixed-point coordinates.
 */
typedef struct {
    nymya_qubit q;
    int64_t    x, y, z, w, v;
} nymya_qpos5d_k;


/**
 * nymya_qpos3d - 3D position struct for a qubit (userspace floating-point).
 * @x, y, z: Spatial coordinates.
 * @q: Associated qubit.
 */
typedef struct {
    double x, y, z;
    nymya_qubit q;
} nymya_qpos3d;

/**
 * nymya_qpos4d - 4D position struct for a qubit (userspace floating-point).
 * @x, y, z, w: 4D spatial coordinates.
 * @q: Associated qubit.
 */
typedef struct {
    double x, y, z, w;
    nymya_qubit q;
} nymya_qpos4d;

/**
 * nymya_qpos5d - 5D position struct for a qubit (userspace floating-point).
 * @x, y, z, w, v: 5D spatial coordinates.
 * @q: Associated qubit.
 */
typedef struct {
    double x, y, z, w, v;
    nymya_qubit q;
} nymya_qpos5d;

// Shared function declarations
int log_symbolic_event(const char* gate, uint64_t id, const char* tag, const char* msg);
int nymya_event_class_syscall_enter(uint64_t syscall_id, uint64_t qubit_id);
int nymya_event_class_syscall_exit(uint64_t syscall_id, uint64_t qubit_id);
int nymya_syscall_event_exit(uint64_t syscall_id, int return_code);
int nymya_syscall_print_exit_funcs(uint64_t syscall_id, int return_code);
int nymya_exit_syscall_print_funcs(uint64_t syscall_id, int return_code);

#ifdef __KERNEL__
// Define the inverse of sqrt(2) in fixed-point for kernel calculations

#define FIXED_POINT_SQRT2_INV_FP (int64_t)(0.7071067811865476 * FIXED_POINT_SCALE)

// Define PI and PI/2 in fixed-point for kernel calculations if not already in nymya.h

#define FIXED_POINT_PI (int64_t)(3.141592653589793 * FIXED_POINT_SCALE)

#define FIXED_POINT_PI_DIV_2 (int64_t)(1.5707963267948966 * FIXED_POINT_SCALE) // M_PI / 2.0

/**
 * fixed_complex_multiply - Multiplies two fixed-point complex numbers and returns a complex_double.
 * @re1: Real part of the first complex number.
 * @im1: Imaginary part of the first complex number.
 * @re2: Real part of the second complex number.
 * @im2: Imaginary part of the second complex number.
 *
 * Multiplies two complex numbers (re1 + im1 * i) and (re2 + im2 * i)
 * using the fixed-point Q32.32 format. The result is also in Q32.32 format.
 *
 * Formula:
 * (re1 + im1 * i) * (re2 + im2 * i) = (re1 * re2 - im1 * im2) + (re1 * im2 + im1 * re2) * i
 *
 * Returns:
 * A complex_double struct containing both the real and imaginary parts of the product.
 * Note: This function is similar to complex_mul but takes individual fixed-point components.
 */
complex_double fixed_complex_multiply(int64_t re1, int64_t im1, int64_t re2, int64_t im2);

#endif

#endif // NYMYA_H




// Quantum-symbolic syscalls

// These declarations should ideally be conditional if their parameter types change
// based on __KERNEL__. They are currently non-conditional here, which might
// lead to issues if their actual implementations (in their respective .c files)
// use different types for kernel vs. userland.
// The conditional declarations were in the 'nymya-function-documentation' immersive,
// which represents the *intended* nymya.h content. This section here is a duplicate
// and should be removed or made conditional. For now, assuming the top #ifndef NYMYA_H
// and the conditional sections within this file are the primary source of truth.


/**
 * nymya_3301_identity_gate - Applies the Identity gate to a single qubit.
 * @q: Pointer to the target qubit.
 *
 * The Identity gate leaves the qubit's state unchanged. It is often used
 * for placeholder operations or to maintain circuit depth.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL.
 */
int nymya_3301_identity_gate(nymya_qubit* q);

/**
 * nymya_3302_global_phase - Applies a global phase shift to a single qubit.
 * @q: Pointer to the target qubit.
 * @theta: The phase angle in radians (double for userland, int64_t fixed-point for kernel).
 *
 * This function applies a global phase shift of $e^{i\theta}$ to the qubit's
 * amplitude. A global phase is generally unobservable but can be important
 * in multi-qubit systems or when considering relative phases.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3302_global_phase(nymya_qubit* q, double theta);
#else
int nymya_3302_global_phase(nymya_qubit* q, int64_t theta);
#endif

/**
 * nymya_3303_pauli_x - Applies the Pauli-X (NOT) gate to a single qubit.
 * @q: Pointer to the target qubit.
 *
 * The Pauli-X gate is equivalent to a classical NOT gate. It flips the
 * amplitude of the |0> state with the |1> state.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL.
 */
int nymya_3303_pauli_x(nymya_qubit* q);

/**
 * nymya_3304_pauli_y - Applies the Pauli-Y gate to a single qubit.
 * @q: Pointer to the target qubit.
 *
 * The Pauli-Y gate is a rotation around the Y-axis of the Bloch sphere by $\pi$ radians.
 * It introduces a phase shift in addition to flipping amplitudes.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL.
 */
int nymya_3304_pauli_y(nymya_qubit* q);

/**
 * nymya_3305_pauli_z - Applies the Pauli-Z gate to a single qubit.
 * @q: Pointer to the target qubit.
 *
 * The Pauli-Z gate applies a phase flip to the |1> state, leaving the |0>
 * state unchanged. It is a rotation around the Z-axis of the Bloch sphere by $\pi$ radians.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL.
 */
int nymya_3305_pauli_z(nymya_qubit* q);

/**
 * nymya_3306_phase_gate - Applies a generic phase gate (S gate) to a single qubit.
 * @q: Pointer to the target qubit.
 *
 * This function applies a specific phase gate, often referred to as the S gate,
 * which applies a phase of $i$ to the |1> state. It is a Z-rotation by $\pi/2$.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL.
 */
int nymya_3306_phase_gate(nymya_qubit* q);

/**
 * nymya_3307_sqrt_x_gate - Applies the square root of Pauli-X gate to a single qubit.
 * @q: Pointer to the target qubit.
 *
 * The $\sqrt{X}$ gate is a gate such that applying it twice is equivalent to applying
 * a Pauli-X gate. It is a rotation around the X-axis by $\pi/2$ radians.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL.
 */
int nymya_3307_sqrt_x_gate(nymya_qubit* q);

/**
 * nymya_3308_hadamard_gate - Applies the Hadamard gate to a single qubit.
 * @q: Pointer to the target qubit.
 *
 * The Hadamard gate creates a superposition state from a basis state.
 * It maps |0> to $(|0> + |1>)/\sqrt{2}$ and |1> to $(|0> - |1>)/\sqrt{2}$.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL.
 */
int nymya_3308_hadamard_gate(nymya_qubit* q);

/**
 * nymya_3309_controlled_not - Applies the Controlled-NOT (CNOT) gate to two qubits.
 * @q_ctrl: Pointer to the control qubit.
 * @q_target: Pointer to the target qubit.
 *
 * The CNOT gate flips the state of the target qubit if and only if the control
 * qubit is in the |1> state.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3309_controlled_not(nymya_qubit* q_ctrl, nymya_qubit* q_target);

/**
 * nymya_3310_anticontrol_not - Applies an Anti-Controlled-NOT gate to two qubits.
 * @q_ctrl: Pointer to the control qubit.
 * @q_target: Pointer to the target qubit.
 *
 * The Anti-Controlled-NOT gate flips the state of the target qubit if and only
 * if the control qubit is in the |0> state.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3310_anticontrol_not(nymya_qubit* q_ctrl, nymya_qubit* q_target);

/**
 * nymya_3311_controlled_z - Applies the Controlled-Z (CZ) gate to two qubits.
 * @q_ctrl: Pointer to the control qubit.
 * @q_target: Pointer to the target qubit.
 *
 * The CZ gate applies a phase flip to the |11> state, leaving other basis
 * states unchanged. It is symmetric, so the control and target roles can be swapped.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3311_controlled_z(nymya_qubit* q_ctrl, nymya_qubit* q_target);

/**
 * nymya_3312_double_controlled_not - Applies the Toffoli (CCNOT) gate to three qubits.
 * @qc1: Pointer to the first control qubit.
 * @qc2: Pointer to the second control qubit.
 * @qt: Pointer to the target qubit.
 *
 * The Toffoli gate flips the state of the target qubit if and only if both
 * control qubits are in the |1> state. It is a universal classical gate.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3312_double_controlled_not(nymya_qubit* qc1, nymya_qubit* qc2, nymya_qubit* qt);

/**
 * nymya_3313_swap - Applies the SWAP gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * The SWAP gate exchanges the states of two qubits.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3313_swap(nymya_qubit* q1, nymya_qubit* q2);

/**
 * nymya_3314_imaginary_swap - Applies the iSWAP gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * The iSWAP gate is a variant of the SWAP gate that also applies a phase shift.
 * It swaps the amplitudes of |01> and |10> states and introduces a phase of $i$.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3314_imaginary_swap(nymya_qubit* q1, nymya_qubit* q2);

/**
 * nymya_3315_phase_shift - Applies a phase shift gate (Rz($\theta$)) to a single qubit.
 * @q: Pointer to the target qubit.
 * @theta: The phase angle in radians (double for userland, int64_t fixed-point for kernel).
 *
 * This function applies a rotation around the Z-axis of the Bloch sphere by an angle $\theta$.
 * It applies a phase of $e^{i\theta}$ to the |1> state.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3315_phase_shift(nymya_qubit* q, double theta);
#else
int nymya_3315_phase_shift(nymya_qubit* q, int64_t theta);
#endif

/**
 * nymya_3316_phase_gate - Applies a specific phase gate (P gate) to a single qubit.
 * @q: Pointer to the target qubit.
 * @phi: The phase angle in radians (double for userland, int64_t fixed-point for kernel).
 *
 * This function applies a general phase gate, which is a rotation around the Z-axis.
 * It maps $|1\rangle \rightarrow e^{i\phi}|1\rangle$.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3316_phase_gate(nymya_qubit* q, double phi);
#else
int nymya_3316_phase_gate(nymya_qubit* q, int64_t phi);
#endif

/**
 * nymya_3317_controlled_phase - Applies a Controlled-Phase (CPHASE) gate to two qubits.
 * @qc: Pointer to the control qubit.
 * @qt: Pointer to the target qubit.
 * @theta: The phase angle in radians (double for userland, int64_t fixed-point for kernel).
 *
 * The CPHASE gate applies a phase of $e^{i\theta}$ to the |11> state if both
 * control and target qubits are in the |1> state.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3317_controlled_phase(nymya_qubit* qc, nymya_qubit* qt, double theta);
#else
int nymya_3317_controlled_phase(nymya_qubit* qc, nymya_qubit* qt, int64_t theta);
#endif

/**
 * nymya_3318_controlled_phase_s - Applies a Controlled-S (CS) gate to two qubits.
 * @qc: Pointer to the control qubit.
 * @qt: Pointer to the target qubit.
 *
 * The CS gate is a specific controlled-phase gate where the phase angle is $\pi/2$.
 * It applies a phase of $i$ to the |11> state.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3318_controlled_phase_s(nymya_qubit* qc, nymya_qubit* qt);

/**
 * nymya_3319_rotate_x - Applies a rotation around the X-axis to a single qubit.
 * @q: Pointer to the target qubit.
 * @theta: The angle of rotation in radians (double for userland, int64_t fixed-point for kernel).
 *
 * This function applies a rotation $R_x(\theta)$ to the qubit's state vector
 * on the Bloch sphere.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3319_rotate_x(nymya_qubit* q, double theta);
#else
int nymya_3319_rotate_x(nymya_qubit* q, int64_t theta); // Kernel version accepts fixed-point
#endif

/**
 * nymya_3320_rotate_y - Applies a rotation around the Y-axis to a single qubit.
 * @q: Pointer to the target qubit.
 * @theta: The angle of rotation in radians (double for userland, int64_t fixed-point for kernel).
 *
 * This function applies a rotation $R_y(\theta)$ to the qubit's state vector
 * on the Bloch sphere.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3320_rotate_y(nymya_qubit* q, double theta);
#else
int nymya_3320_rotate_y(nymya_qubit* q, int64_t theta); // Kernel version accepts fixed-point
#endif

/**
 * nymya_3321_rotate_z - Applies a rotation around the Z-axis to a single qubit.
 * @q: Pointer to the target qubit.
 * @theta: The angle of rotation in radians (double for userland, int64_t fixed-point for kernel).
 *
 * This function applies a rotation $R_z(\theta)$ to the qubit's state vector
 * on the Bloch sphere.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3321_rotate_z(nymya_qubit* q, double theta);
#else
int nymya_3321_rotate_z(nymya_qubit* q, int64_t theta); // Kernel version accepts fixed-point
#endif

/**
 * nymya_3322_xx_interaction - Applies an XX interaction gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @theta: The interaction angle (double for userland, int64_t fixed-point for kernel).
 *
 * This gate implements an interaction Hamiltonian proportional to $\sigma_x \otimes \sigma_x$.
 * It is often used in quantum simulation.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3322_xx_interaction(nymya_qubit* q1, nymya_qubit* q2, double theta);
#else
int nymya_3322_xx_interaction(nymya_qubit* q1, nymya_qubit* q2, int64_t theta);
#endif

/**
 * nymya_3323_yy_interaction - Applies a YY interaction gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @theta: The interaction angle (double for userland, int64_t fixed-point for kernel).
 *
 * This gate implements an interaction Hamiltonian proportional to $\sigma_y \otimes \sigma_y$.
 * It is often used in quantum simulation.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3323_yy_interaction(nymya_qubit* q1, nymya_qubit* q2, double theta);
#else
int nymya_3323_yy_interaction(nymya_qubit* q1, nymya_qubit* q2, int64_t theta);
#endif

/**
 * nymya_3324_zz_interaction - Applies a ZZ interaction gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @theta: The interaction angle (double for userland, int64_t fixed-point for kernel).
 *
 * This gate implements an interaction Hamiltonian proportional to $\sigma_z \otimes \sigma_z$.
 * It is often used in quantum simulation.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3324_zz_interaction(nymya_qubit* q1, nymya_qubit* q2, double theta);
#else
int nymya_3324_zz_interaction(nymya_qubit* q1, nymya_qubit* q2, int64_t theta);
#endif

/**
 * nymya_3325_xyz_entangle - Applies an XYZ entangling gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @theta: The entangling angle (double for userland, int64_t fixed-point for kernel).
 *
 * This gate implements a specific entangling operation involving X, Y, and Z Pauli matrices.
 * The exact transformation depends on the underlying implementation.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3325_xyz_entangle(nymya_qubit* q1, nymya_qubit* q2, double theta);
#else
int nymya_3325_xyz_entangle(nymya_qubit* q1, nymya_qubit* q2, int64_t theta);
#endif

/**
 * nymya_3326_sqrt_swap - Applies the square root of SWAP gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * The $\sqrt{SWAP}$ gate is a two-qubit gate such that applying it twice
 * is equivalent to applying a full SWAP gate. It is an entangling gate.
 *
 * Returns:
 * - 0 on success.
 * - -1 if either qubit pointer is NULL.
 */
int nymya_3326_sqrt_swap(nymya_qubit* q1, nymya_qubit* q2);

/**
 * nymya_3327_sqrt_iswap - Applies the square root of iSWAP gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * The $\sqrt{iSWAP}$ gate is a two-qubit entangling gate. Applying it twice
 * is equivalent to applying an iSWAP gate.
 *
 * Returns:
 * - 0 on success.
 * - -1 if either qubit pointer is NULL.
 */
int nymya_3327_sqrt_iswap(nymya_qubit* q1, nymya_qubit* q2);

/**
 * nymya_3328_swap_pow - Applies an interpolated SWAP^alpha gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @alpha: The interpolation parameter (double for userland, int64_t fixed-point for kernel). A value of 0 results in an Identity gate,
 * and a value of 1 results in a full SWAP gate.
 *
 * This gate allows for continuous interpolation between the Identity and SWAP gates.
 *
 * Returns:
 * - 0 on success.
 * - -1 if either qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3328_swap_pow(nymya_qubit* q1, nymya_qubit* q2, double alpha);
#else
int nymya_3328_swap_pow(nymya_qubit* q1, nymya_qubit* q2, int64_t alpha);
#endif

/**
 * nymya_3329_fredkin - Applies a Fredkin (Controlled-SWAP) gate to three qubits.
 * @q_ctrl: Pointer to the control qubit.
 * @q1: Pointer to the first target qubit.
 * @q2: Pointer to the second target qubit.
 *
 * The Fredkin gate swaps the states of @q1 and @q2 if and only if the control
 * qubit @q_ctrl is in the |1> state. It is a universal classical gate.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3329_fredkin(nymya_qubit* q_ctrl, nymya_qubit* q1, nymya_qubit* q2);

/**
 * nymya_3330_rotate - Applies a rotation gate to a single qubit around a specified axis.
 * @q: Pointer to the qubit to rotate.
 * @axis: The axis of rotation ('X', 'Y', or 'Z', case-insensitive).
 * @theta: The angle of rotation in radians (double for userland, int64_t fixed-point for kernel).
 *
 * This function acts as a wrapper, calling the appropriate single-qubit rotation
 * gate ($R_x$, $R_y$, or $R_z$) based on the specified axis and angle.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit pointer is NULL or an unknown axis is specified.
 */
#ifndef __KERNEL__
int nymya_3330_rotate(nymya_qubit* q, char axis, double theta);
#else
int nymya_3330_rotate(nymya_qubit* q, char axis, int64_t theta);
#endif

/**
 * nymya_3331_barenco - Applies a Barenco (Controlled-Controlled-Controlled-NOT) gate to three qubits.
 * @q1: Pointer to the first control qubit.
 * @q2: Pointer to the second control qubit.
 * @q3: Pointer to the third target qubit.
 *
 * This function implements a Barenco gate, which is a generalized Toffoli gate.
 * It flips the state of @q3 if @q1 and @q2 are both in the |1> state.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3331_barenco(nymya_qubit* q1, nymya_qubit* q2, nymya_qubit* q3);

/**
 * nymya_3332_berkeley - Applies a Berkeley gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @theta: The gate parameter (double for userland, int64_t fixed-point for kernel).
 *
 * This function implements a two-qubit Berkeley gate, which is a parameterized
 * entangling gate. The exact transformation depends on the parameter $\theta$.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3332_berkeley(nymya_qubit* q1, nymya_qubit* q2, double theta);
#else
int nymya_3332_berkeley(nymya_qubit* q1, nymya_qubit* q2, int64_t theta);
#endif

/**
 * nymya_3333_c_v - Applies a Controlled-V gate to two qubits.
 * @qc: Pointer to the control qubit.
 * @qt: Pointer to the target qubit.
 *
 * This function applies a Controlled-V gate, where V is the square root of Pauli-X.
 * It applies $\sqrt{X}$ to the target qubit if the control qubit is in the |1> state.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3333_c_v(nymya_qubit* qc, nymya_qubit* qt);

/**
 * nymya_3334_core_entangle - Applies a core entangling gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function implements a fundamental entangling gate, often a CNOT or
 * a similar two-qubit interaction that creates entanglement between the qubits.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3334_core_entangle(nymya_qubit* q1, nymya_qubit* q2);

/**
 * nymya_3335_dagwood - Applies a Dagwood gate to three qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @q3: Pointer to the third qubit.
 *
 * This function implements a three-qubit Dagwood gate. The specific
 * quantum operation performed is defined by the gate's internal logic.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3335_dagwood(nymya_qubit* q1, nymya_qubit* q2, nymya_qubit* q3);

/**
 * nymya_3336_echo_cr - Applies an Echo Cross-Resonance gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @theta: The interaction angle (double for userland, int64_t fixed-point for kernel).
 *
 * This function implements an Echo Cross-Resonance gate, a common entangling
 * gate used in superconducting qubit architectures. It applies a controlled
 * rotation with refocusing pulses.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3336_echo_cr(nymya_qubit* q1, nymya_qubit* q2, double theta);
#else
int nymya_3336_echo_cr(nymya_qubit* q1, nymya_qubit* q2, int64_t theta);
#endif

/**
 * nymya_3337_fermion_sim - Applies a fermionic simulation gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function implements a gate used for simulating fermionic interactions,
 * often derived from Jordan-Wigner or Bravyi-Kitaev transformations.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3337_fermion_sim(nymya_qubit* q1, nymya_qubit* q2);

/**
 * nymya_3338_givens - Applies a Givens rotation gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @theta: The rotation angle (double for userland, int64_t fixed-point for kernel).
 *
 * This function implements a Givens rotation, which is a unitary operation
 * used in linear algebra, adapted for two-qubit systems. It can be used
 * for operations like quantum Fourier transform components.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
#ifndef __KERNEL__
int nymya_3338_givens(nymya_qubit* q1, nymya_qubit* q2, double theta);
#else
int nymya_3338_givens(nymya_qubit* q1, nymya_qubit* q2, int64_t theta);
#endif

/**
 * nymya_3339_magic - Applies a Magic gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function implements a two-qubit Magic gate, which is an entangling
 * gate often used in quantum information theory for its specific properties.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3339_magic(nymya_qubit* q1, nymya_qubit* q2);

/**
 * nymya_3340_sycamore - Applies a Sycamore gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function implements a Sycamore gate, a specific type of entangling
 * gate used in Google's Sycamore quantum processor. It is a fixed two-qubit gate.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3340_sycamore(nymya_qubit* q1, nymya_qubit* q2);

/**
 * nymya_3341_cz_swap - Applies a CZ-SWAP gate to two qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function implements a composite gate involving Controlled-Z and SWAP operations.
 * The exact sequence of operations is defined internally.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3341_cz_swap(nymya_qubit* q1, nymya_qubit* q2);

/**
 * nymya_3342_deutsch - Applies a Deutsch-Jozsa oracle gate.
 * @q1: Pointer to the input qubit.
 * @q2: Pointer to the ancilla qubit.
 * @f: A function pointer representing the oracle (e.g., a function that applies
 * a Pauli-X gate to q2 if q1 is |1> for a balanced function).
 *
 * This function implements the core operation of the Deutsch-Jozsa algorithm.
 * It applies the oracle function 'f' to the qubits.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL or the function pointer is invalid.
 */
int nymya_3342_deutsch(nymya_qubit* q1, nymya_qubit* q2, void (*f)(nymya_qubit*));

/**
 * nymya_3343_margolis - Applies a Margolis gate to three qubits.
 * @qc1: Pointer to the first control qubit.
 * @qc2: Pointer to the second control qubit.
 * @qt: Pointer to the target qubit.
 *
 * This function implements a three-qubit Margolis gate, a type of controlled
 * rotation gate. The specific operation is defined by the gate's internal logic.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3343_margolis(nymya_qubit* qc1, nymya_qubit* qc2, nymya_qubit* qt);

/**
 * nymya_3344_peres - Applies a Peres gate to three qubits.
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 * @q3: Pointer to the third qubit.
 *
 * This function implements a three-qubit Peres gate, which is equivalent to
 * a Toffoli gate followed by a CNOT gate. It is a reversible logic gate.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3344_peres(nymya_qubit* q1, nymya_qubit* q2, nymya_qubit* q3);

/**
 * nymya_3345_cf_swap - Applies a Controlled-Fredkin (CCSWAP) gate to three qubits.
 * @qc: Pointer to the control qubit.
 * @q1: Pointer to the first target qubit.
 * @q2: Pointer to the second target qubit.
 *
 * This function implements a Controlled-Fredkin gate (also known as a
 * Controlled-Controlled-SWAP). It swaps @q1 and @q2 if the control qubit @qc
 * is in the |1> state.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3345_cf_swap(nymya_qubit* qc, nymya_qubit* q1, nymya_qubit* q2);

/**
 * nymya_3346_triangular_lattice - Applies entangling operations based on a triangular lattice.
 * @q1: Pointer to the first qubit in a triangular unit.
 * @q2: Pointer to the second qubit in a triangular unit.
 * @q3: Pointer to the third qubit in a triangular unit.
 *
 * This function applies entangling gates (e.g., CNOTs) between qubits
 * arranged in a triangular lattice configuration. The specific interactions
 * depend on the internal logic.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL.
 */
int nymya_3346_triangular_lattice(nymya_qubit* q1, nymya_qubit* q2, nymya_qubit* q3);

/**
 * nymya_3347_hexagonal_lattice - Applies entangling operations based on a hexagonal lattice.
 * @q: An array of 6 pointers to qubits forming a hexagonal unit.
 *
 * This function applies entangling gates between qubits arranged in a
 * hexagonal lattice configuration. The specific interactions depend on the internal logic.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit array is NULL.
 */
int nymya_3347_hexagonal_lattice(nymya_qubit* q[6]);

/**
 * nymya_3348_hex_rhombi_lattice - Applies entangling operations based on a hex-rhombi lattice.
 * @q: An array of 7 pointers to qubits forming a hex-rhombi unit.
 *
 * This function applies entangling gates between qubits arranged in a
 * hex-rhombi lattice configuration. The specific interactions depend on the internal logic.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit array is NULL.
 */
int nymya_3348_hex_rhombi_lattice(nymya_qubit* q[7]);

/**
 * nymya_3349_tessellated_triangles - Applies entangling operations across a tessellated triangular lattice.
 * @q: An array of pointers to qubits.
 * @count: The total number of qubits in the tessellated lattice.
 *
 * This function simulates interactions across a larger structure formed by
 * tessellating triangular units, applying entangling gates between neighboring qubits.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit array is NULL or count is insufficient.
 */
int nymya_3349_tessellated_triangles(nymya_qubit* q[], size_t count);

/**
 * nymya_3350_tessellated_hexagons - Applies entangling operations across a tessellated hexagonal lattice.
 * @q: An array of pointers to qubits.
 * @count: The total number of qubits in the tessellated lattice.
 *
 * This function simulates interactions across a larger structure formed by
 * tessellating hexagonal units, applying entangling gates between neighboring qubits.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit array is NULL or count is insufficient.
 */
int nymya_3350_tessellated_hexagons(nymya_qubit* q[], size_t count);

/**
 * nymya_3351_tessellated_hex_rhombi - Applies entangling operations across a tessellated hex-rhombi lattice.
 * @q: An array of pointers to qubits.
 * @count: The total number of qubits in the tessellated lattice.
 *
 * This function simulates interactions across a larger structure formed by
 * tessellating hex-rhombi units, applying entangling gates between neighboring qubits.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit array is NULL or count is insufficient.
 */
int nymya_3351_tessellated_hex_rhombi(nymya_qubit* q[], size_t count);

/**
 * nymya_3352_e8_group - Applies quantum operations based on the E8 Lie group structure.
 * @q: An array of 8 pointers to qubits.
 *
 * This function applies complex entangling operations derived from the
 * symmetries and properties of the E8 Lie group, potentially for advanced
 * quantum error correction or simulation.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit array is NULL.
 */
int nymya_3352_e8_group(nymya_qubit* q[8]);

/**
 * nymya_3353_flower_of_life - Applies quantum operations based on the Flower of Life geometry.
 * @q: An array of pointers to qubits.
 * @count: The total number of qubits.
 *
 * This function applies entangling operations to qubits arranged according to
 * the geometric principles of the Flower of Life, potentially for novel quantum
 * architectures or simulations.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit array is NULL or count is insufficient.
 */
int nymya_3353_flower_of_life(nymya_qubit* q[], size_t count);

/**
 * nymya_3354_metatron_cube - Applies quantum operations based on Metatron's Cube geometry.
 * @q: An array of pointers to qubits.
 * @count: The total number of qubits.
 *
 * This function applies entangling operations to qubits arranged according to
 * the geometric principles of Metatron's Cube, potentially for novel quantum
 * architectures or simulations.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the qubit array is NULL or count is insufficient.
 */
int nymya_3354_metatron_cube(nymya_qubit* q[], size_t count);

/**
 * nymya_3355_fcc_lattice - Applies quantum operations on qubits in a Face-Centered Cubic (FCC) lattice.
 * @qubits: An array of nymya_qpos3d structures, representing qubits with their 3D positions.
 * @count: The number of qubits in the array.
 *
 * This function simulates quantum operations (e.g., Hadamard, CNOT) on qubits
 * arranged in an FCC crystal lattice structure in 3D space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if the qubits array is NULL or count is less than the minimum for an FCC unit.
 */
int nymya_3355_fcc_lattice(nymya_qpos3d qubits[], size_t count);

/**
 * nymya_3356_hcp_lattice - Applies quantum operations on qubits in a Hexagonal Close-Packed (HCP) lattice.
 * @qubits: An array of nymya_qpos3d structures, representing qubits with their 3D positions.
 * @count: The number of qubits in the array.
 *
 * This function simulates quantum operations (e.g., Hadamard, CNOT) on qubits
 * arranged in an HCP crystal lattice structure in 3D space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if the qubits array is NULL or count is less than the minimum for an HCP unit.
 */
int nymya_3356_hcp_lattice(nymya_qpos3d qubits[], size_t count);

/**
 * nymya_3357_e8_projected_lattice - Applies quantum operations on qubits in an E8 projected lattice.
 * @qubits: An array of nymya_qpos3d structures, representing qubits with their 3D positions.
 * @count: The number of qubits in the array.
 *
 * This function simulates quantum operations (e.g., Hadamard, CNOT) on qubits
 * arranged in a 3D projection of an E8 lattice structure.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if the qubits array is NULL or count is less than the minimum for an E8 projection.
 */
int nymya_3357_e8_projected_lattice(nymya_qpos3d qubits[], size_t count);

/**
 * nymya_3358_d4_lattice - Applies quantum operations on qubits in a D4 lattice.
 * @qubits: An array of nymya_qpos4d structures, representing qubits with their 4D positions.
 * @count: The number of qubits in the array.
 *
 * This function simulates quantum operations (e.g., Hadamard, CNOT) on qubits
 * arranged in a D4 lattice structure in 4D space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if the qubits array is NULL or count is less than the minimum for a D4 unit.
 */
int nymya_3358_d4_lattice(nymya_qpos4d qubits[], size_t count);

/**
 * nymya_3359_b5_lattice - Applies quantum operations on qubits in a B5 lattice.
 * @qubits: An array of nymya_qpos5d structures, representing qubits with their 5D positions.
 * @count: The number of qubits in the array.
 *
 * This function simulates quantum operations (e.g., Hadamard, CNOT) on qubits
 * arranged in a B5 lattice structure in 5D space.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if the qubits array is NULL or count is less than the minimum for a B5 unit.
 */
int nymya_3359_b5_lattice(nymya_qpos5d qubits[], size_t count);

/**
 * nymya_3360_e5_projected_lattice - Applies quantum operations on qubits in an E5 projected lattice.
 * @qubits: An array of nymya_qpos5d structures, representing qubits with their 5D positions.
 * @count: The number of qubits in the array.
 *
 * This function simulates quantum operations (e.g., Hadamard, CNOT) on qubits
 * arranged in a 5D projection of an E5 lattice structure.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if the qubits array is NULL or count is less than the minimum for an E5 projection.
 */
int nymya_3360_e5_projected_lattice(nymya_qpos5d qubits[], size_t count);

/**
 * nymya_3361_qrng_range - Generates quantum random numbers within a specified range.
 * @out: Pointer to an array where the generated random numbers will be stored.
 * @min: The minimum value for the generated random numbers (inclusive).
 * @max: The maximum value for the generated random numbers (inclusive).
 * @count: The number of random numbers to generate.
 *
 * This function leverages quantum properties to generate true random numbers
 * within a given range. The underlying quantum process is assumed to be
 * accessible and properly measured.
 *
 * Returns:
 * - 0 on success.
 * - -1 if the output array is NULL or if min > max.
 */
int nymya_3361_qrng_range(uint64_t* out, uint64_t min, uint64_t max, size_t count);



// Shared complex math macros
#define nymya_cexp(theta) complex_exp_i(theta)
#define nymya_cmul(a, b) complex_mul((a), (b))


// Gate macros (for clean user-facing syntax)
#define identity(q) nymya_3301_identity_gate(q)
#define NYMYA_IDENTITY_GATE_CODE 3301

#define global_phase(q, theta) nymya_3302_global_phase(q, theta)
#define NYMYA_GLOBAL_PHASE_CODE 3302

#define pauli_x(q) nymya_3303_pauli_x(q)
#define NYMYA_PAULI_X_CODE 3303

#define pauli_y(q) nymya_3304_pauli_y(q)
#define NYMYA_PAULI_Y_CODE 3304

#define pauli_z(q) nymya_3305_pauli_z(q)
#define NYMYA_PAULI_Z_CODE 3305

#define phase_s(q) nymya_3306_phase_gate(q)
#define NYMYA_PHASE_S_CODE 3306

#define sqrt_x(q) nymya_3307_sqrt_x_gate(q)
#define NYMYA_SQRT_X_CODE 3307

#define hadamard(q) nymya_3308_hadamard_gate(q)
#define NYMYA_HADAMARD_CODE 3308

#define cnot(qc, qt) nymya_3309_controlled_not(qc, qt)
#define NYMYA_CNOT_CODE 3309

#define acnot(qc, qt) nymya_3310_anticontrol_not(qc, qt)
#define NYMYA_ACNOT_CODE 3310

#define cz(qc, qt) nymya_3311_controlled_z(qc, qt)
#define NYMYA_CZ_CODE 3311

#define dcnot(q1, q2, qt) nymya_3312_double_controlled_not(q1, q2, qt)
#define NYMYA_DCNOT_CODE 3312

#define nymya_swap(q1, q2) nymya_3313_swap(q1, q2)
#define NYMYA_SWAP_CODE 3313

#define imswap(q1, q2) nymya_3314_imaginary_swap(q1, q2)
#define NYMYA_IMSWAP_CODE 3314

#define phase_shift(q, theta) nymya_3315_phase_shift(q, theta)
#define NYMYA_PHASE_SHIFT_CODE 3315

#define phase_gate(q, phi) nymya_3316_phase_gate(q, phi)
#define NYMYA_PHASE_GATE_CODE 3316

#define cphase(qc, qt, theta) nymya_3317_controlled_phase(qc, qt, theta)
#define NYMYA_CPHASE_CODE 3317

#define cphase_s(qc, qt) nymya_3318_controlled_phase_s(qc, qt)
#define NYMYA_CPHASE_S_CODE 3318

#define rotate_x(q, theta) nymya_3319_rotate_x(q, theta)
#define NYMYA_ROTATE_X_CODE 3319

#define rotate_y(q, theta) nymya_3320_rotate_y(q, theta)
#define NYMYA_ROTATE_Y_CODE 3320

#define rotate_z(q, theta) nymya_3321_rotate_z(q, theta)
#define NYMYA_ROTATE_Z_CODE 3321

#define xx(q1, q2, theta) nymya_3322_xx_interaction(q1, q2, theta)
#define NYMYA_XX_CODE 3322

#define yy(q1, q2, theta) nymya_3323_yy_interaction(q1, q2, theta)
#define NYMYA_YY_CODE 3323

#define zz(q1, q2, theta) nymya_3324_zz_interaction(q1, q2, theta)
#define NYMYA_ZZ_CODE 3324

#define xyz(q1, q2, theta) nymya_3325_xyz_entangle(q1, q2, theta)
#define NYMYA_XYZ_CODE 3325

#define sqrt_swap(q1, q2) nymya_3326_sqrt_swap(q1, q2)
#define NYMYA_SQRT_SWAP_CODE 3326

#define sqrt_iswap(q1, q2) nymya_3327_sqrt_iswap(q1, q2)
#define NYMYA_SQRT_ISWAP_CODE 3327

#define swap_pow(q1, q2, a) nymya_3328_swap_pow(q1, q2, a)
#define NYMYA_SWAP_POW_CODE 3328

#define fredkin(qc, q1, q2) nymya_3329_fredkin(qc, q1, q2)
#define NYMYA_FREDKIN_CODE 3329

#define rotate(q, a, t) nymya_3330_rotate(q, a, t)
#define NYMYA_ROTATE_CODE 3330

#define barenco(q1, q2, q3) nymya_3331_barenco(q1, q2, q3)
#define NYMYA_BARENCO_CODE 3331

#define berkeley(q1, q2, t) nymya_3332_berkeley(q1, q2, t)
#define NYMYA_BERKELEY_CODE 3332

#define c_v(qc, qt) nymya_3333_c_v(qc, qt)
#define NYMYA_C_V_CODE 3333

#define core_entangle(q1, q2) nymya_3334_core_entangle(q1, q2)
#define NYMYA_CORE_ENTANGLE_CODE 3334

#define dagwood(q1, q2, q3) nymya_3335_dagwood(q1, q2, q3)
#define NYMYA_DAGWOOD_CODE 3335

#define echo_cr(q1, q2, t) nymya_3336_echo_cr(q1, q2, t)
#define NYMYA_ECHO_CR_CODE 3336

#define fermion_sim(q1, q2) nymya_3337_fermion_sim(q1, q2)
#define NYMYA_FERMION_SIM_CODE 3337

#define givens(q1, q2, t) nymya_3338_givens(q1, q2, t)
#define NYMYA_GIVENS_CODE 3338

#define magic(q1, q2) nymya_3339_magic(q1, q2)
#define NYMYA_MAGIC_CODE 3339

#define sycamore(q1, q2) nymya_3340_sycamore(q1, q2)
#define NYMYA_SYCAMORE_CODE 3340

#define cz_swap(q1, q2) nymya_3341_cz_swap(q1, q2)
#define NYMYA_CZ_SWAP_CODE 3341

#define deutsch(q1, q2, f) nymya_3342_deutsch(q1, q2, f)
#define NYMYA_DEUTSCH_CODE 3342

#define margolis(qc1, qc2, qt) nymya_3343_margolis(qc1, qc2, qt)
#define NYMYA_MARGOLIS_CODE 3343

#define peres(q1, q2, q3) nymya_3344_peres(q1, q2, q3)
#define NYMYA_PERES_CODE 3344

#define cf_swap(qc, q1, q2) nymya_3345_cf_swap(qc, q1, q2)
#define NYMYA_CF_SWAP_CODE 3345

#define triangular_lattice(q1, q2, q3) nymya_3346_triangular_lattice(q1, q2, q3)
#define NYMYA_TRIANGULAR_LATTICE_CODE 3346

#define hexagonal_lattice(q) nymya_3347_hexagonal_lattice(q)
#define NYMYA_HEXAGONAL_LATTICE_CODE 3347

#define hex_rhombi_lattice(q) nymya_3348_hex_rhombi_lattice(q)
#define NYMYA_HEX_RHOMBI_LATTICE_CODE 3348

#define tessellate_triangles(q, n) nymya_3349_tessellated_triangles(q, n)
#define NYMYA_TESS_TRIANGLES_CODE 3349

#define tessellate_hexagons(q, n) nymya_3350_tessellated_hexagons(q, n)
#define NYMYA_TESS_HEXAGONS_CODE 3350

#define tessellate_hex_rhombi(q, n) nymya_3351_tessellated_hex_rhombi(q, n)
#define NYMYA_TESS_HEX_RHOMBI_CODE 3351

#define e8_group(q) nymya_3352_e8_group(q)
#define NYMYA_E8_GROUP_CODE 3352

#define flower_of_life(q, n) nymya_3353_flower_of_life(q, n)
#define NYMYA_FLOWER_OF_LIFE_CODE 3353

#define metatron_cube(q, n) nymya_3354_metatron_cube(q, n)
#define NYMYA_METATRON_CUBE_CODE 3354

#define fcc_lattice(q, n) nymya_3355_fcc_lattice(q, n)
#define NYMYA_FCC_LATTICE_CODE 3355

#define hcp_lattice(q, n) nymya_3356_hcp_lattice(q, n)
#define NYMYA_HCP_LATTICE_CODE 3356

#define e8_projected_lattice(q, n) nymya_3357_e8_projected_lattice(q, n)
#define NYMYA_E8_PROJECTED_CODE 3357

#define d4_lattice(q, n) nymya_3358_d4_lattice(q, n)
#define NYMYA_D4_LATTICE_CODE 3358

#define b5_lattice(q, n) nymya_3359_b5_lattice(q, n)
#define NYMYA_B5_LATTICE_CODE 3359

#define e5_projected_lattice(q, n) nymya_3360_e5_projected_lattice(q, n)
#define NYMYA_E5_PROJECTED_CODE 3360

#define qrng_range(out, min, max, count) nymya_3361_qrng_range(out, min, max, count)
#define NYMYA_QRNG_CODE 3361

