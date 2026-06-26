#ifndef CONSTRUCTIBLE_FUNCS_H
#define CONSTRUCTIBLE_FUNCS_H

/* ── constructible_funcs.h — Time-Constructible Functions ──
 * f is time-constructible if ∃ TM that on input 1^n halts
 * in EXACTLY f(n) steps.
 *
 * L1 Definitions, L3 Math Structures, L4 Gap Theorem
 *
 * Without constructibility, the Gap Theorem (Borodin 1972)
 * shows ∃f with TIME(f) = TIME(2^f), breaking the hierarchy. */

#include <time.h>

/* ══════════════════════════════════════════
 * Time-Constructible Function Type */

/* A function f: N → N. Time-constructible if computable
 * in O(f(n)) time on input 1^n. */
typedef long long (*ConstructibleFunc)(int n);

/* ══════════════════════════════════════════
 * Standard Time-Constructible Functions */

/* f(n) = n (linear) */
long long tc_linear(int n);

/* f(n) = n * log_2(n) */
long long tc_n_log_n(int n);

/* f(n) = n^2 (quadratic) */
long long tc_quadratic(int n);

/* f(n) = n^k for any k ≥ 1 */
long long tc_polynomial(int n, int k);

/* f(n) = 2^n (exponential) */
long long tc_exponential(int n);

/* f(n) = 2^{n^2} (doubly exponential) */
long long tc_double_exponential(int n);

/* f(n) = n! (factorial) */
long long tc_factorial(int n);

/* ══════════════════════════════════════════
 * Verification */

/* Verify f is time-constructible: can we compute f(n)
 * within c·f(n) steps for some constant c? */
int tc_verify_constructible(ConstructibleFunc f, int n, double *ratio);

/* Test: is floor(sqrt(n)) time-constructible?
 * Answer: NO. We cannot compute it in O(sqrt(n)) steps. */
int tc_sqrt_is_not_constructible(void);

/* Test: Busy Beaver function is NOT time-constructible
 * (it grows faster than any computable function). */
int tc_busy_beaver_not_constructible(void);

/* ══════════════════════════════════════════
 * Gap Theorem */

/* Demonstrate: for any computable g, ∃f such that TIME(f) = TIME(g∘f).
 * This pathological f is NOT time-constructible.
 * The existence shows constructibility is NECESSARY. */
void gap_theorem_demonstrate(int n_samples);

/* ══════════════════════════════════════════
 * Constructible Class Membership */

/* Test if a complexity class defined by bound f is
 * "constructible" (the bound itself is time-constructible). */
int tc_class_is_constructible(ConstructibleFunc bound);

/* List all time-constructible bounds up to a given level.
 * Returns number of constructible functions found. */
int tc_list_constructible(int max_k, ConstructibleFunc *out, int out_cap);

#endif
