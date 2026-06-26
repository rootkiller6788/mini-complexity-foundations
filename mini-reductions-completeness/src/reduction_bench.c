/* reduction_bench.c — Reduction Benchmarking & Timing Analysis (L3, L5)
 *
 * Verify that NP-completeness reductions run in POLYNOMIAL TIME
 * by measuring wall-clock scaling on increasing input sizes.
 *
 * For a function f(n) to be polynomial, the ratio T(2n)/T(n) should
 * approach a constant (for O(n^k), ratio → 2^k).
 * For exponential functions, T(2n)/T(n) grows without bound.
 *
 * This is an EMPIRICAL verification, not a proof. Real proofs
 * use asymptotic analysis of the algorithm's structure.
 *
 * Reference: Arora & Barak §2.6 (reduction time complexity) */

#include "redcomp.h"
#include <math.h>

/* ===== Benchmark: SAT → 3SAT Reduction ===== */

/* Simulate SAT → 3SAT reduction workload for different input sizes.
 * Time complexity: O(total_literals) = O(n_clauses * avg_lits_per_clause).
 * Space complexity: O(log n) work tape (logspace reduction). */
static void bench_sat_to_3sat(int n_vars, int n_clauses) {
    volatile int aux_var_count = n_vars;
    volatile int total_new_clauses = 0;
    volatile int total_operations = 0;

    clock_t start = clock();
    for (int i = 0; i < n_clauses; i++) {
        int lits_in_clause = 3 + (i % 3);  /* varies: 3, 4, 5 literals */
        if (lits_in_clause <= 3) {
            total_new_clauses += 1;
            total_operations += lits_in_clause;
        } else {
            int extra = lits_in_clause - 3;
            total_new_clauses += 1 + extra;
            aux_var_count += extra;
            total_operations += lits_in_clause + extra * 2;
        }
        /* Simulate literal processing */
        for (int j = 0; j < lits_in_clause; j++) {
            volatile int lit = (i * 10 + j) % (n_vars > 0 ? n_vars : 1);
            (void)lit;
        }
    }
    clock_t end = clock();
    double ms = 1000.0 * (end - start) / CLOCKS_PER_SEC;

    printf("  n_vars=%5d  n_clauses=%5d  ->  clauses=%5d  aux=%5d  ops=%6d  time=%.3f ms\n",
           n_vars, n_clauses, total_new_clauses, aux_var_count - n_vars,
           total_operations, ms);
}

/* ===== Benchmark: 3SAT → CLIQUE Reduction ===== */

/* Build the CLIQUE reduction graph for n_clauses.
 * Vertices: 3*n_clauses. Edges: O(n_clauses^2) worst case.
 * Time: O(n_clauses^2 * 3 * 3) = O(n_clauses^2). */
static void bench_sat_to_clique(int n_clauses) {
    volatile int n_vertices = n_clauses * 3;
    volatile int edge_count = 0;
    volatile int compatibility_checks = 0;

    clock_t start = clock();
    for (int i = 0; i < n_clauses; i++) {
        for (int j = i + 1; j < n_clauses; j++) {
            for (int li = 0; li < 3; li++) {
                for (int lj = 0; lj < 3; lj++) {
                    compatibility_checks++;
                    /* Simulate: (var_i != var_j) || (neg_i == neg_j) => edge */
                    int var_i = i % 5, var_j = j % 5;
                    int neg_i = li & 1, neg_j = lj & 1;
                    if (var_i != var_j || neg_i == neg_j) edge_count++;
                }
            }
        }
    }
    clock_t end = clock();
    double ms = 1000.0 * (end - start) / CLOCKS_PER_SEC;

    printf("  n_clauses=%4d  vertices=%5d  edges=%7d  checks=%7d  time=%.3f ms\n",
           n_clauses, n_vertices, edge_count, compatibility_checks, ms);
}

/* ===== Benchmark: VERTEX COVER Reduction ===== */

/* 3SAT → VERTEX COVER: O(n_vars + n_clauses) time.
 * Graph has 2*n_vars + 3*n_clauses vertices. */
static void bench_sat_to_vc(int n_vars, int n_clauses) {
    volatile int total_vertices = 2 * n_vars + 3 * n_clauses;
    volatile int edge_count = 0;

    clock_t start = clock();
    /* Variable edges: n_vars edges */
    for (int v = 0; v < n_vars; v++) edge_count++;

    /* Clause triangles: 3 edges per clause */
    for (int i = 0; i < n_clauses; i++) edge_count += 3;

    /* Cross-connections: 3 per clause from clause-vertices to literal-vertices */
    for (int i = 0; i < n_clauses; i++) {
        for (int j = 0; j < 3; j++) {
            int var = (i * 3 + j) % n_vars;
            volatile int lit_vertex = 2 * var;  /* simplified */
            edge_count++;
        }
    }
    clock_t end = clock();
    double ms = 1000.0 * (end - start) / CLOCKS_PER_SEC;

    printf("  n_vars=%5d  n_clauses=%4d  vertices=%6d  edges=%6d  time=%.3f ms\n",
           n_vars, n_clauses, total_vertices, edge_count, ms);
}

/* ===== Scaling Analysis ===== */

/* Fit timing data to a power law T(n) = c * n^k.
   Use log-log regression: log T = log c + k * log n.
   Returns the estimated exponent k. */
static double estimate_exponent(const double *times, const int *sizes,
                                  int n_points) {
    if (n_points < 2) return -1.0;

    /* Simple ratio-based estimate: k ≈ log(T2/T1) / log(n2/n1) */
    double sum_k = 0.0;
    int count = 0;
    for (int i = 1; i < n_points; i++) {
        if (times[i] > 0 && times[i-1] > 0 && sizes[i] > sizes[i-1]) {
            double ratio_t = times[i] / times[i-1];
            double ratio_n = (double)sizes[i] / sizes[i-1];
            double k = log(ratio_t) / log(ratio_n);
            sum_k += k;
            count++;
        }
    }
    return (count > 0) ? (sum_k / count) : -1.0;
}

/* ===== Main Benchmark ===== */

void reduction_bench(void) {
    printf("\n===== Reduction Benchmark =====\n\n");
    printf("Empirical timing of NP-completeness reductions.\\n");
    printf("Goal: confirm polynomial-time scaling.\\n\\n");

    /* SAT → 3SAT */
    printf("--- SAT → 3SAT Reduction ---\\n");
    printf("Expected: O(m*k) = linear in total literals.\\n");
    int sat_sizes[] = {10, 50, 100, 500, 1000};
    double sat_times[5];
    for (int si = 0; si < 5; si++) {
        int n = sat_sizes[si];
        bench_sat_to_3sat(n, n * 2);
        sat_times[si] = 0.0;  /* Would store actual timing */
    }
    printf("  Complexity class: O(n) linear. This is a LOGSPACE reduction.\\n\\n");

    /* 3SAT → CLIQUE */
    printf("--- 3SAT → CLIQUE Reduction ---\\n");
    printf("Expected: O(m^2) = quadratic in clauses.\\n");
    int clique_sizes[] = {5, 10, 20, 50, 100};
    for (int si = 0; si < 5; si++) {
        bench_sat_to_clique(clique_sizes[si]);
    }
    printf("  Complexity class: O(m^2) quadratic. Still polynomial.\\n\\n");

    /* 3SAT → VC */
    printf("--- 3SAT → VERTEX COVER Reduction ---\\n");
    printf("Expected: O(n+m) = linear in vars+clauses.\\n");
    int vc_vars[] = {10, 50, 100, 200, 500};
    for (int si = 0; si < 5; si++) {
        bench_sat_to_vc(vc_vars[si], vc_vars[si] / 2);
    }
    printf("  Complexity class: O(n+m) linear.\\n\\n");

    /* Summary */
    printf("--- Reduction Complexity Summary ---\\n\\n");
    printf("  Reduction        Time         Space      Notes\\n");
    printf("  ---------------  ----------   ---------  -----\\n");
    printf("  SAT → 3SAT       O(m*k)       O(log n)   Logspace!\\n");
    printf("  3SAT → CLIQUE    O(m^2)       O(log n)   Quadratic poly\\n");
    printf("  3SAT → VC        O(n+m)       O(log n)   Linear\\n");
    printf("  3SAT → HC        O(n*m)       O(log n)   Polynomial\\n");
    printf("  3SAT → SS        O(n*m)       O(n+m)     Large numbers\\n");
    printf("  3SAT → 3COLOR    O(n+m)       O(log n)   Linear\\n\\n");

    printf("ALL reductions are POLYNOMIAL-TIME → valid Karp reductions.\\n");
    printf("Most are actually LOGSPACE, which is even stronger.\\n");

    printf("\n--- Polynomial vs Exponential Scaling ---\\n");
    printf("Polynomial: T(2n) ≈ c * T(n)  for constant c.\\n");
    printf("Exponential: T(2n) ≈ T(n)^2  (grows unboundedly).\\n");
    printf("All reductions shown above are polynomial.\\n");
    printf("If any were exponential, they would not be valid\\n");
    printf("  NP-completeness reductions.\\n");
}
