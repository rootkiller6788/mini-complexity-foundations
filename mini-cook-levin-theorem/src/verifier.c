/*
 * verifier.c — Polynomial-Time Verifiers for NP Languages
 *
 * L2 Core Concepts:
 *   NP = class of languages with polynomial-time verifiers.
 *   L in NP iff exists poly-time V such that:
 *     x in L <=> exists certificate y with |y| <= poly(|x|) and V(x,y)=1
 *
 * This module implements verifiers for:
 *   1. SAT: assignment is the certificate
 *   2. 3SAT: assignment is the certificate
 *   3. CLIQUE: set of vertices is the certificate
 *   4. VERTEX-COVER: set of vertices is the certificate
 *
 * All verifiers run in O(poly(|instance|)) time.
 *
 * Reference: Sipser section 7.3, Arora-Barak section 2.1
 * Courses: MIT 6.045, Stanford CS254, Berkeley CS172
 */

#include "cook_levin.h"
#include "sat.h"
#include "reduction.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* ================================================================
 * SAT Verifier
 * ================================================================
 *
 * Certificate: Assignment of length n_vars
 * Verification: Check each clause has at least one true literal
 * Complexity: O(total_literals) = O(|phi|)
 */

int sat_verify_wrapper(const void* instance, const void* certificate) {
    const CNF* cnf = (const CNF*)instance;
    const Assignment assign = (const Assignment)certificate;
    return sat_verify(cnf, assign);
}

/* ================================================================
 * CLIQUE Verifier
 * ================================================================
 *
 * Certificate: Array of k vertex indices
 * Verification: Check all pairs in the certificate are adjacent
 * Complexity: O(k^2)
 */

int clique_verify_wrapper(const void* instance, const void* certificate) {
    const Graph* g = (const Graph*)instance;
    const int* clique = (const int*)certificate;
    if (!g || !clique) return 0;

    /* First element of certificate is k */
    int k = clique[0];
    if (k <= 0 || k > g->n_vertices) return 0;

    /* Check that all pairs (clique[i], clique[j]) are adjacent */
    for (int i = 1; i <= k; i++) {
        for (int j = i + 1; j <= k; j++) {
            int u = clique[i], v = clique[j];
            if (u < 0 || u >= g->n_vertices || v < 0 || v >= g->n_vertices)
                return 0;
            if (!g->adj_matrix[u * g->n_vertices + v])
                return 0;
        }
    }
    return 1;
}

/* ================================================================
 * VERTEX-COVER Verifier
 * ================================================================
 *
 * Certificate: Array of vertex indices forming the cover
 * Verification: Check that every edge has at least one endpoint in the cover
 * Complexity: O(|V| + |E|)
 */

int vertexcover_verify_wrapper(const void* instance, const void* certificate) {
    const VCGraph* g = (const VCGraph*)instance;
    const int* cover = (const int*)certificate;
    if (!g || !cover) return 0;

    int k = cover[0];  /* first element is the cover size */

    /* Build a boolean array of covered vertices */
    int* in_cover = (int*)calloc((size_t)g->n_vertices, sizeof(int));
    for (int i = 1; i <= k; i++) {
        int v = cover[i];
        if (v >= 0 && v < g->n_vertices)
            in_cover[v] = 1;
    }

    /* Check every edge is covered */
    for (int e = 0; e < g->n_edges; e++) {
        if (!in_cover[g->edge_u[e]] && !in_cover[g->edge_v[e]]) {
            free(in_cover);
            return 0;
        }
    }

    free(in_cover);
    return 1;
}

/* ================================================================
 * Generic NP Verifier Framework
 * ================================================================ */

/*
 * Check that a certificate is valid for an NP language instance.
 * Also verifies that the certificate length is bounded by poly(|instance|).
 */
int np_verify(const void* instance, const void* certificate,
              int (*verifier)(const void*, const void*),
              int cert_length, int poly_bound) {
    if (cert_length > poly_bound) return 0;  /* certificate too long */
    return verifier(instance, certificate);
}

/* ================================================================
 * Certificate Generation for Testing
 * ================================================================ */

/*
 * Generate a certificate for a SAT instance.
 * This ASSUMES the instance is SAT and runs the solver to find an assignment.
 */
int* sat_generate_certificate(CNF* cnf) {
    if (!cnf) return NULL;
    Assignment assign = assignment_create(cnf->n_vars);
    if (!sat_solve_dpll(cnf, assign)) {
        assignment_free(assign);
        return NULL;
    }
    /* Return the assignment (caller must free) */
    return assign;
}

/*
 * Convert a SAT satisfying assignment to the DIMACS output format.
 * Prints "SAT" followed by the assignment on one line.
 */
void sat_print_solution(const CNF* cnf, const Assignment assign) {
    if (!cnf || !assign) return;
    printf("SAT\n");
    for (int v = 0; v < cnf->n_vars; v++) {
        int val = assign[v];
        printf("%d ", (val > 0) ? (v + 1) : -(v + 1));
    }
    printf("0\n");
}

/* ================================================================
 * Verification Benchmarks
 * ================================================================ */

/*
 * Time how long verification takes for a given instance+certificate pair.
 * Returns milliseconds per verification.
 */
double bench_sat_verification(const CNF* cnf, const Assignment assign, int trials) {
    if (!cnf || !assign) return 0.0;
    clock_t t0 = clock();
    for (int i = 0; i < trials; i++) {
        sat_verify(cnf, assign);
    }
    clock_t t1 = clock();
    return (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0 / (double)trials;
}

/*
 * Compare NTM simulation time vs SAT verification time.
 * Demonstrates that NP = "easy to verify, (possibly) hard to solve".
 */
void compare_solve_vs_verify(CNF* cnf) {
    if (!cnf) return;
    printf("\n=== Solve vs Verify Comparison ===\n");

    /* Solve (exponential worst-case) */
    clock_t t0 = clock();
    Assignment assign = assignment_create(cnf->n_vars);
    int sat_result = sat_solve_dpll(cnf, assign);
    clock_t t1 = clock();
    double solve_ms = (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0;

    printf("Solve: %s (%.3f ms)\n", sat_result ? "SAT" : "UNSAT", solve_ms);

    /* Verify (polynomial time) */
    t0 = clock();
    int verify_result = sat_verify(cnf, assign);
    t1 = clock();
    double verify_ms = (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0;

    printf("Verify: %s (%.6f ms)\n", verify_result ? "VALID" : "INVALID", verify_ms);
    printf("Ratio (solve/verify): %.1fx\n", verify_ms > 0 ? solve_ms / verify_ms : 0.0);

    assignment_free(assign);
}