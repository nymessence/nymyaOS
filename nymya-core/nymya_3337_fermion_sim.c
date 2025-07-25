// src/nymya_3337_fermion_sim.c
//
// This file implements the nymya_3337_fermion_sim syscall, which applies a
// two-qubit fermionic simulation gate. This gate typically involves swapping
// the states of two qubits and applying a phase shift, mimicking an exchange
// interaction in fermionic systems.
//
// The implementation includes both userland and kernel-space versions,
// ensuring proper header inclusion and fixed-point arithmetic for kernel operations.

#include "nymya.h" // Common definitions like complex_double, nymya_qubit, and fixed-point helpers

#ifndef __KERNEL__
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
    #include <stdio.h>    // Userland: For standard I/O (e.g., in log_symbolic_event)
#define __NR_nymya_3337_fermion_sim NYMYA_FERMION_SIM_CODE

    #include <stdlib.h>  // Userland: For general utilities
    #include <math.h>    // Userland: For complex math functions like cabs
    #include <complex.h> // Userland: For _Complex double type and I macro
#else
    #include <linux/kernel.h>   // Kernel: For pr_err and general kernel functions
    #include <linux/syscalls.h> // Kernel: For SYSCALL_DEFINE macros
    #include <linux/uaccess.h>  // Kernel: For copy_from_user, copy_to_user
    #include <linux/errno.h>    // Kernel: For error codes like -EINVAL, -EFAULT
    // No math.h or complex.h in kernel; fixed-point math assumed for amplitude operations
#endif

#ifndef __KERNEL__

/**
 * nymya_3337_fermion_sim - Applies a two-qubit fermionic simulation gate (userland).
 * @q1: Pointer to the first qubit.
 * @q2: Pointer to the second qubit.
 *
 * This function simulates a fermionic exchange interaction. It performs a SWAP
 * operation between the two qubits and then applies a global phase of -1 to
 * the first qubit's amplitude. This effectively implements a permutation with
 * a sign change, characteristic of fermionic statistics.
 *
 * Returns:
 * - 0 on success.
 * - -1 if any qubit pointer is NULL (invalid input).
 */
int nymya_3337_fermion_sim(nymya_qubit* q1, nymya_qubit* q2) {
    // Basic null pointer check
    if (!q1 || !q2) return -1;

    // Apply the SWAP gate using the userland nymya_swap macro (which calls nymya_3313_swap)
    nymya_swap(q1, q2);

    // Apply a phase shift of -1 to the first qubit's amplitude
    // In userland, amplitude is _Complex double, so direct multiplication by -1 is fine.
    q1->amplitude *= -1;

    // Log the symbolic event for traceability
    log_symbolic_event("FERMION_SIM", q1->id, q1->tag, "Fermionic exchange");
    return 0;
}

#else // __KERNEL__

/**
 * SYSCALL_DEFINE2(nymya_3337_fermion_sim) - Kernel syscall for fermionic simulation gate.
 * @user_q1: User-space pointer to the first qubit structure.
 * @user_q2: User-space pointer to the second qubit structure.
 *
 * This syscall copies qubit data from user space to kernel space, applies the
 * fermionic simulation gate logic using kernel-space functions and fixed-point
 * arithmetic, and then copies the modified data back to user space.
 * The gate involves a SWAP operation followed by a phase shift of -1 on the first qubit.
 *
 * Returns:
 * - 0 on success.
 * - -EINVAL if any user qubit pointer is NULL.
 * - -EFAULT if copying data to/from user space fails.
 * - Error code from underlying gate operations (e.g., nymya_3313_swap).
 */
SYSCALL_DEFINE2(nymya_3337_fermion_sim,
    struct nymya_qubit __user *, user_q1,
    struct nymya_qubit __user *, user_q2) {

    struct nymya_qubit k_q1, k_q2; // Kernel-space copies of qubits
    int ret = 0; // Return value for syscall

    // 1. Check for null pointers from user-space
    if (!user_q1 || !user_q2) {
        pr_err("nymya_3337_fermion_sim: Null user qubit pointer(s)\n");
        return -EINVAL; // Invalid argument
    }

    // 2. Copy the qubit structures from user space to kernel space
    if (copy_from_user(&k_q1, user_q1, sizeof(k_q1))) {
        pr_err("nymya_3337_fermion_sim: Failed to copy k_q1 from user\n");
        return -EFAULT; // Bad address
    }
    if (copy_from_user(&k_q2, user_q2, sizeof(k_q2))) {
        pr_err("nymya_3337_fermion_sim: Failed to copy k_q2 from user\n");
        return -EFAULT; // Bad address
    }

    // 3. Apply the fermionic simulation logic for kernel space
    // Call the kernel version of the SWAP gate
    ret = nymya_3313_swap(&k_q1, &k_q2);
    if (ret) {
        pr_err("nymya_3337_fermion_sim: SWAP gate failed, error %d\n", ret);
        return ret; // Propagate error from SWAP gate
    }

    // Apply a phase shift of -1 to the first qubit's amplitude
    // For a complex number (re + i*im), multiplying by -1 results in (-re - i*im).
    k_q1.amplitude.re = -k_q1.amplitude.re;
    k_q1.amplitude.im = -k_q1.amplitude.im;

    // 4. Log the symbolic event for traceability
    log_symbolic_event("FERMION_SIM", k_q1.id, k_q1.tag, "Fermionic exchange");

    // 5. Copy the modified qubits back to user space
    if (copy_to_user(user_q1, &k_q1, sizeof(k_q1))) {
        pr_err("nymya_3337_fermion_sim: Failed to copy k_q1 to user\n");
        ret = -EFAULT; // Bad address
    }
    if (copy_to_user(user_q2, &k_q2, sizeof(k_q2))) {
        pr_err("nymya_3337_fermion_sim: Failed to copy k_q2 to user\n");
        ret = -EFAULT; // Bad address
    }

    return ret; // Return 0 on success, or error code if any copy_to_user failed
}

#endif

