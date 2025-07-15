# 🌀 NymyaOS Quantum-Symbolic Gates

Welcome to the gate reference for `nymyaOS`. These gates form the symbolic and functional core of the Nymya Quantum Syscall Layer, bridging classical and quantum logic via a mystical–mathematical design.

Each gate corresponds to a syscall function and has a symbolic code for identification within the runtime.

---

## 🧠 Core Qubit Gates

| Macro               | Function Name                      | Code  | Description                        |
|---------------------|----------------------------------|-------|----------------------------------|
| `identity(q)`       | `nymya_3301_identity_gate`        | 3301  | Identity (no operation)           |
| `global_phase(q,t)` | `nymya_3302_global_phase`         | 3302  | Global phase shift                |
| `pauli_x(q)`        | `nymya_3303_pauli_x`              | 3303  | Pauli-X (bit flip)               |
| `pauli_y(q)`        | `nymya_3304_pauli_y`              | 3304  | Pauli-Y                         |
| `pauli_z(q)`        | `nymya_3305_pauli_z`              | 3305  | Pauli-Z (phase flip)             |
| `hadamard(q)`       | `nymya_3308_hadamard_gate`        | 3308  | Hadamard (H) gate                |
| `phase_s(q)`        | `nymya_3306_phase_gate`           | 3306  | S-gate (π/2 phase)               |
| `phase_gate(q,φ)`   | `nymya_3316_phase_gate`           | 3316  | General phase rotation           |
| `phase_shift(q,θ)`  | `nymya_3315_phase_shift`          | 3315  | Arbitrary phase shift            |

---

## 🔄 Rotation Gates

| Macro                   | Function Name                 | Code  |
|-------------------------|------------------------------|-------|
| `rotate_x(q, θ)`        | `nymya_3319_rotate_x`         | 3319  |
| `rotate_y(q, θ)`        | `nymya_3320_rotate_y`         | 3320  |
| `rotate_z(q, θ)`        | `nymya_3321_rotate_z`         | 3321  |
| `rotate(q, axis, θ)`    | `nymya_3330_rotate`           | 3330  |

---

## 🕸 Entangling & Controlled Gates

| Macro                     | Function Name                  | Code  |
|---------------------------|-------------------------------|-------|
| `cnot(qc, qt)`            | `nymya_3309_controlled_not`    | 3309  |
| `acnot(qc, qt)`           | `nymya_3310_anticontrol_not`   | 3310  |
| `cz(qc, qt)`              | `nymya_3311_controlled_z`      | 3311  |
| `cphase(qc, qt, θ)`       | `nymya_3317_controlled_phase`  | 3317  |
| `cphase_s(qc, qt)`        | `nymya_3318_controlled_phase_s`| 3318  |
| `dcnot(q1, q2, qt)`       | `nymya_3312_double_controlled_not` | 3312  |

---

## 💠 Swap and Permutations

| Macro                      | Function Name                 | Code  |
|----------------------------|------------------------------|-------|
| `swap(q1, q2)`             | `nymya_3313_swap`             | 3313  |
| `imswap(q1, q2)`           | `nymya_3314_imaginary_swap`   | 3314  |
| `sqrt_swap(q1, q2)`        | `nymya_3326_sqrt_swap`        | 3326  |
| `sqrt_iswap(q1, q2)`       | `nymya_3327_sqrt_iswap`       | 3327  |
| `swap_pow(q1, q2, α)`      | `nymya_3328_swap_pow`         | 3328  |
| `cz_swap(q1, q2)`          | `nymya_3341_cz_swap`          | 3341  |
| `cf_swap(qc, q1, q2)`      | `nymya_3345_cf_swap`          | 3345  |

---

## ⚛️ Higher Order Logic Gates

| Macro                     | Function Name               | Code  |
|---------------------------|----------------------------|-------|
| `fredkin(qc, q1, q2)`     | `nymya_3329_fredkin`       | 3329  |
| `barenco(q1, q2, q3)`     | `nymya_3331_barenco`       | 3331  |
| `peres(q1, q2, q3)`       | `nymya_3344_peres`         | 3344  |
| `dagwood(q1, q2, q3)`     | `nymya_3335_dagwood`       | 3335  |
| `margolis(qc1, qc2, qt)`  | `nymya_3343_margolis`      | 3343  |

---

## 🔁 Interaction Gates

| Macro                   | Function Name             | Code  |
|-------------------------|---------------------------|-------|
| `xx(q1, q2, θ)`         | `nymya_3322_xx_interaction` | 3322  |
| `yy(q1, q2, θ)`         | `nymya_3323_yy_interaction` | 3323  |
| `zz(q1, q2, θ)`         | `nymya_3324_zz_interaction` | 3324  |
| `xyz(q1, q2, θ)`        | `nymya_3325_xyz_entangle` | 3325  |
| `core_entangle(q1, q2)` | `nymya_3334_core_entangle` | 3334  |

---

## 🧬 Advanced / Exotic

| Macro                      | Function Name           | Code  |
|----------------------------|------------------------|-------|
| `berkeley(q1, q2, θ)`      | `nymya_3332_berkeley`   | 3332  |
| `c_v(qc, qt)`              | `nymya_3333_c_v`        | 3333  |
| `echo_cr(q1, q2, θ)`       | `nymya_3336_echo_cr`    | 3336  |
| `fermion_sim(q1, q2)`      | `nymya_3337_fermion_sim`| 3337  |
| `givens(q1, q2, θ)`        | `nymya_3338_givens`     | 3338  |
| `magic(q1, q2)`            | `nymya_3339_magic`      | 3339  |
| `sycamore(q1, q2)`         | `nymya_3340_sycamore`   | 3340  |
| `deutsch(q1, q2, f)`       | `nymya_3342_deutsch`    | 3342  |

---

## 🌌 Sacred Geometry Gates

| Macro                      | Function Name             | Code  |
|----------------------------|--------------------------|-------|
| `triangular_lattice(...)`  | `nymya_3346_triangular_lattice` | 3346  |
| `hexagonal_lattice(...)`   | `nymya_3347_hexagonal_lattice`  | 3347  |
| `hex_rhombi_lattice(...)`  | `nymya_3348_hex_rhombi_lattice` | 3348  |
| `tessellate_triangles(...)`| `nymya_3349_tessellated_triangles` | 3349  |
| `tessellate_hexagons(...)` | `nymya_3350_tessellated_hexagons` | 3350  |
| `tessellate_hex_rhombi(...)`| `nymya_3351_tessellated_hex_rhombi` | 3351  |
| `flower_of_life(...)`      | `nymya_3353_flower_of_life` | 3353  |
| `metatron_cube(...)`       | `nymya_3354_metatron_cube` | 3354  |

---

## 🧿 Lattice-Based and High-D Symmetry

| Macro                      | Function Name               | Code  |
|----------------------------|----------------------------|-------|
| `fcc_lattice(...)`         | `nymya_3355_fcc_lattice`   | 3355  |
| `hcp_lattice(...)`         | `nymya_3356_hcp_lattice`   | 3356  |
| `e8_group(q)`              | `nymya_3352_e8_group`      | 3352  |
| `e8_projected_lattice(...)`| `nymya_3357_e8_projected_lattice` | 3357  |
| `d4_lattice(...)`          | `nymya_3358_d4_lattice`    | 3358  |
| `b5_lattice(...)`          | `nymya_3359_b5_lattice`.   | 3359  |
