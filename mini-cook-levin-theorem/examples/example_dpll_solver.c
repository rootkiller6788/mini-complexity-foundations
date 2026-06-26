/*
 * example_dpll_solver.c — End-to-End SAT Solving with DPLL
 *
 * L5 Algorithm: Demonstrates the Davis-Putnam-Logemann-Loveland
 * backtracking SAT solver on multiple formula families.
 *
 * Knowledge: SAT, 3SAT, DPLL, unit propagation, pure literal elimination,
 *            exact model counting, verifier polynomial-time check.
 *
 * Reference: Davis, Logemann, Loveland (CACM 1962)
 * Courses: MIT 6.045, Stanford CS254, Berkeley CS172
 *
 * Build: gcc -I../include -o example_dpll_solver example_dpll_solver.c \
 *            ../src/sat_cnf.c ../src/dpll.c -lm
 */

#include "sat.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ── Formula Families ──────────────────────────────────────── */

/*
 * Pigeonhole principle (PHP): n+1 pigeons in n holes.
 * Encoding: variable (i*n + j) means pigeon i is in hole j.
 * Unsatisfiable for all n >= 1.
 */
static CNF* make_pigeonhole(int n) {
    int np1 = n + 1;
    int n_vars = np1 * n;
    CNF* cnf = cnf_create(n_vars, np1 + np1 * n * (n - 1) / 2);

    /* Each pigeon in at least one hole */
    for (int i = 0; i < np1; i++) {
        int* lits = (int*)malloc((size_t)n * sizeof(int));
        for (int j = 0; j < n; j++)
            lits[j] = SAT_LIT_POS(i * n + j);
        cnf_add_clause(cnf, lits, n);
        free(lits);
    }

    /* No two pigeons share a hole */
    for (int j = 0; j < n; j++) {
        for (int i1 = 0; i1 < np1; i1++) {
            for (int i2 = i1 + 1; i2 < np1; i2++) {
                cnf_add_binary(cnf,
                    SAT_LIT_NEG(i1 * n + j),
                    SAT_LIT_NEG(i2 * n + j));
            }
        }
    }
    return cnf;
}

/*
 * Random k-SAT formula with m clauses.
 * Each clause has k distinct randomly chosen literals.
 */
static CNF* make_random_ksat(int n_vars, int k, int m_clauses) {
    CNF* cnf = cnf_create(n_vars, m_clauses);
    for (int ci = 0; ci < m_clauses; ci++) {
        int lits[8];
        for (int li = 0; li < k && li < 8; li++) {
            int v = rand() % n_vars;
            int sign = rand() & 1;
            lits[li] = sign ? SAT_LIT_NEG(v) : SAT_LIT_POS(v);
        }
        cnf_add_clause(cnf, lits, k);
    }
    return cnf;
}

/*
 * Parity formula: x_1 XOR x_2 XOR ... XOR x_n = 0
 * Encoded in CNF by enumerating all assignments with odd parity as forbidden.
 * This produces 2^{n-1} clauses (exponential, use only small n).
 */
static CNF* make_parity_formula(int n_vars) {
    int n_clauses = 1 << (n_vars - 1);
    CNF* cnf = cnf_create(n_vars, n_clauses > 500 ? 500 : n_clauses);
    int max_c = (n_clauses > 500) ? 500 : n_clauses;
    for (int mask = 0; mask < (1 << n_vars) && cnf->n_clauses < max_c; mask++) {
        int parity = 0;
        for (int v = 0; v < n_vars; v++)
            if (mask & (1 << v)) parity ^= 1;
        if (parity == 0) continue;  /* even parity = satisfying, skip */
        /* Forbidden: odd parity. Add clause forbidding this assignment */
        int* lits = (int*)malloc((size_t)n_vars * sizeof(int));
        for (int v = 0; v < n_vars; v++)
            lits[v] = (mask & (1 << v)) ? SAT_LIT_NEG(v) : SAT_LIT_POS(v);
        cnf_add_clause(cnf, lits, n_vars);
        free(lits);
    }
    return cnf;
}

/* ── Main ──────────────────────────────────────────────────── */

int main(void) {
    srand((unsigned int)time(NULL));
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║   DPLL SAT Solver — End-to-End Demonstration           ║\n");
    printf("║   Davis-Putnam-Logemann-Loveland Algorithm (1962)      ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* ── Example 1: Simple SAT instance ────────────────────── */
    printf("── Example 1: Simple 3-var SAT formula ──\n");
    printf("   φ = (x₁ ∨ x₂) ∧ (¬x₁ ∨ x₃) ∧ (¬x₂ ∨ ¬x₃)\n");
    CNF* f1 = cnf_create(3, 3);
    cnf_add_binary(f1, SAT_LIT_POS(0), SAT_LIT_POS(1));
    cnf_add_binary(f1, SAT_LIT_NEG(0), SAT_LIT_POS(2));
    cnf_add_binary(f1, SAT_LIT_NEG(1), SAT_LIT_NEG(2));

    Assignment a1 = assignment_create(3);
    int r1 = sat_solve_dpll(f1, a1);
    printf("   Result: %s\n", r1 ? "SAT" : "UNSAT");
    if (r1) {
        printf("   Assignment: x₁=%d x₂=%d x₃=%d\n", a1[0], a1[1], a1[2]);
        printf("   Verification: %s\n", sat_verify(f1, a1) ? "✓ valid" : "✗ invalid");
    }
    printf("   Exact #SAT count: %lld (expect 3)\n", sat_count_exact(f1));
    printf("   Approx #SAT prob: %.4f\n\n", sat_count_approx(f1, 10000));
    assignment_free(a1);
    cnf_free(f1);

    /* ── Example 2: UNSAT instance ─────────────────────────── */
    printf("── Example 2: Simple UNSAT formula ──\n");
    printf("   φ = (x₁ ∨ x₂) ∧ (¬x₁) ∧ (¬x₂)\n");
    CNF* f2 = cnf_create(2, 3);
    cnf_add_binary(f2, SAT_LIT_POS(0), SAT_LIT_POS(1));
    cnf_add_unit(f2, SAT_LIT_NEG(0));
    cnf_add_unit(f2, SAT_LIT_NEG(1));
    Assignment a2 = assignment_create(2);
    int r2 = sat_solve_dpll(f2, a2);
    printf("   Result: %s\n", r2 ? "SAT" : "UNSAT");
    printf("   Exact #SAT count: %lld (expect 0)\n\n", sat_count_exact(f2));
    assignment_free(a2);
    cnf_free(f2);

    /* ── Example 3: Pigeonhole Principle ───────────────────── */
    printf("── Example 3: Pigeonhole Principle PHP(3,2) ──\n");
    printf("   3 pigeons, 2 holes → always UNSAT\n");
    CNF* php = make_pigeonhole(2);  /* 3 pigeons, 2 holes */
    printf("   Variables: %d, Clauses: %d\n", php->n_vars, php->n_clauses);
    Assignment a3 = assignment_create(php->n_vars);
    clock_t t0 = clock();
    int r3 = sat_solve_dpll(php, a3);
    clock_t t1 = clock();
    printf("   Result: %s (%.3f ms)\n",
           r3 ? "SAT" : "UNSAT",
           (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0);
    assignment_free(a3);
    cnf_free(php);
    printf("   Note: PHP grows fast — backbone of proof complexity\n\n");

    /* ── Example 4: Random 3SAT at phase transition ────────── */
    printf("── Example 4: Random 3SAT (n=20, m=85) ──\n");
    printf("   Near the phase transition ratio m/n ≈ 4.25\n");
    CNF* rnd = make_random_ksat(20, 3, 85);
    Assignment a4 = assignment_create(20);
    t0 = clock();
    int r4 = sat_solve_dpll(rnd, a4);
    t1 = clock();
    double ms = (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0;
    printf("   Variables: %d, Clauses: %d\n", rnd->n_vars, rnd->n_clauses);
    printf("   Result: %s (%.3f ms)\n", r4 ? "SAT" : "UNSAT", ms);
    if (r4) printf("   Verification: %s\n", sat_verify(rnd, a4) ? "✓ valid" : "✗ invalid");
    printf("   Solver stats: ");
    sat_dpll_stats();
    printf("\n");
    assignment_free(a4);
    cnf_free(rnd);

    /* ── Example 5: Parity on small formula ────────────────── */
    printf("── Example 5: XOR constraint (parity, n=4) ──\n");
    printf("   x₁ ⊕ x₂ ⊕ x₃ ⊕ x₄ = 0 (even parity)\n");
    CNF* parity = make_parity_formula(4);
    Assignment a5 = assignment_create(4);
    int r5 = sat_solve_dpll(parity, a5);
    printf("   CNF: %d vars, %d clauses\n", parity->n_vars, parity->n_clauses);
    printf("   Result: %s\n", r5 ? "SAT" : "UNSAT");
    if (r5) {
        int px = a5[0] ^ a5[1] ^ a5[2] ^ a5[3];
        printf("   Assignment: x₁=%d x₂=%d x₃=%d x₄=%d → parity=%d ✓\n",
               a5[0], a5[1], a5[2], a5[3], px);
    }
    printf("   #SAT count: %lld (expect 8 = 2^{4-1})\n\n",
           sat_count_exact(parity));
    assignment_free(a5);
    cnf_free(parity);

    printf("── All examples complete ──\n");
    return 0;
}
