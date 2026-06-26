#ifndef REDUCTION_METRICS_H
#define REDUCTION_METRICS_H

#include "redcomp.h"

/* ===== Reduction Verification Metrics (L3, L4) =====
 *
 * A valid reduction A ≤_p B must satisfy three properties:
 *   (1) Completeness: x ∈ A  ⇒  f(x) ∈ B
 *   (2) Soundness:    x ∉ A  ⇒  f(x) ∉ B
 *   (3) Efficiency:   f is computable in polynomial time.
 *
 * This module provides tools for measuring and verifying these properties
 * on finite instances, and for benchmarking reduction performance. */

/* Result of verifying a reduction on a finite test set */
typedef struct {
    int total_tests;           /* Number of test instances */
    int completeness_pass;     /* x in A => f(x) in B  (should be all) */
    int soundness_pass;        /* x not in A => f(x) not in B (should be all) */
    int completeness_fail;     /* false negatives */
    int soundness_fail;        /* false positives */
    double total_time_sec;     /* Total time for all tests */
    double avg_time_per_test;  /* Average time per test instance */
    int is_polynomial;         /* 1 if timing data fits polynomial scaling */
    double estimated_exponent; /* Estimated time exponent k in O(n^k) */
} ReductionVerificationResult;

/* ===== Timing and Scaling Analysis ===== */

/* Measure the running time of a reduction function f on inputs 1..max_n.
   Stores timing data in array times[0..max_n-1]. Returns max time in ms. */
double measure_reduction_timing(int (*f)(int), int max_n, double *times);

/* Fit the timing data to a power law: T(n) = c * n^k.
   Returns the R^2 goodness-of-fit. k and c are output parameters. */
double fit_power_law(const double *times, int n_points,
                      double *k, double *c);

/* Classify reduction as polynomial or exponential based on timing data.
   Uses growth-rate test: if time doubles less than 4x when input doubles,
   it's polynomial. Returns 1 for polynomial, 0 for exponential. */
int classify_growth(const double *times, int n_points);

/* ===== Space Complexity Measurement ===== */

/* Measure WORKSPACE (not counting input/output) used by a reduction.
   Uses a simulated stack depth counter. Returns peak workspace in bytes.
   For logspace verification: should be O(log n). */
long measure_reduction_workspace(int (*f)(int), int max_n);

/* Check if workspace usage is logarithmic: W(n) ≤ c * log2(n).
   Returns 1 if logspace, 0 otherwise. */
int is_logspace_workspace(const long *space_data, int n_points, double c);

/* ===== Correctness Score (L4) ===== */

/* Compute a correctness score (0.0 to 1.0) for a reduction on a test set.
   1.0 = perfectly preserves answers on all test instances.
   Score = (completeness_pass + soundness_pass) / (2 * total_tests). */
double reduction_correctness_score(const ReductionVerificationResult *res);

/* Generate test instances for reduction verification.
   For SAT-based reductions: generate random CNF formulas.
   n_instances: number of test cases.
   max_vars: maximum number of variables per instance.
   formulas: output array of strings (NULL-terminated). */
int generate_sat_instances(int n_instances, int max_vars, char ***formulas);

/* ===== Reduction Property Testing ===== */

/* Test reduction transitivity: if A≤B and B≤C, verify that
   the composed reduction A≤C is also correct. */
int test_transitivity(int (*f_ab)(int), int (*f_bc)(int),
                      int (*dec_A)(int), int (*dec_B)(int),
                      int (*dec_C)(int), int max_n);

/* Test that a reduction is MANY-ONE (not Turing).
   Checks that f makes exactly one oracle call per input. */
int test_many_one_property(int (*reduce)(int), int test_input);

/* Print a formatted verification result report */
void print_verification_report(const ReductionVerificationResult *res);

#endif
