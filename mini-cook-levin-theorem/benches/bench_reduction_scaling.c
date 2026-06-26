/*
 * bench_reduction_scaling.c — Empirical Complexity Scaling Benchmarks
 *
 * L2/L5: Measures the empirical scaling of:
 *   1. Cook-Levin reduction: variables and clauses vs. time bound T
 *   2. DPLL solver: solve time vs. number of variables
 *   3. SAT → 3SAT reduction: time vs. formula size
 *   4. CLIQUE construction: time vs. clause count
 *
 * Reference: Arora-Barak, empirical measurements
 * Courses: MIT 6.841, Berkeley CS278, Stanford CS254
 *
 * Build: see Makefile (make benches)
 */

#include "cook_levin.h"
#include "turing_machine.h"
#include "sat.h"
#include "reduction.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* ── Timing helper ─────────────────────────────────────────── */
static double ms_since(clock_t start) {
    return (double)(clock() - start) / CLOCKS_PER_SEC * 1000.0;
}

/* ── Benchmark 1: Cook-Levin scaling ───────────────────────── */
static void bench_cook_levin_scaling(void) {
    printf("── Benchmark 1: Cook-Levin Reduction Scaling ──\n");
    printf("  T   |  P  |  vars  | clauses | time_ms\n");
    printf("──────┼─────┼────────┼─────────┼─────────\n");

    TuringMachine* tm = tm_create_contains_11();
    Symbol input[] = {0, 1, 1, 0};

    for (int T = 2; T <= 32; T *= 2) {
        clock_t t0 = clock();
        CLReduction* red = cook_levin_reduce_detailed(tm, input, 4, T);
        double ms = ms_since(t0);
        int P = T + 4 + 2;
        printf(" %3d  | %3d | %6d | %7d | %7.3f\n",
               T, P, red->n_vars, red->n_clauses, ms);
        cl_reduction_free(red);
    }
    tm_free(tm);
    printf("\n");
}

/* ── Benchmark 2: DPLL solver scaling ──────────────────────── */
static void bench_dpll_scaling(void) {
    printf("── Benchmark 2: DPLL Solver Scaling (Random 3SAT) ──\n");
    printf("  n   |  m   | result | time_ms  | nodes   | backtracks\n");
    printf("──────┼──────┼────────┼──────────┼─────────┼───────────\n");

    for (int n_vars = 5; n_vars <= 25; n_vars += 5) {
        int m = (int)(n_vars * 4.25);  /* phase transition ratio */
        CNF* cnf = cnf_create(n_vars, m);
        for (int ci = 0; ci < m; ci++) {
            int lits[3];
            for (int li = 0; li < 3; li++) {
                int v = rand() % n_vars;
                lits[li] = (rand() & 1) ? SAT_LIT_NEG(v) : SAT_LIT_POS(v);
            }
            cnf_add_clause(cnf, lits, 3);
        }

        Assignment assign = assignment_create(n_vars);
        clock_t t0 = clock();
        int result = sat_solve_dpll(cnf, assign);
        double ms = ms_since(t0);
        printf(" %3d  | %4d | %-6s | %8.3f |         |\n",
               n_vars, m, result ? "SAT" : "UNSAT", ms);
        assignment_free(assign);
        cnf_free(cnf);
    }
    /* Print final stats */
    printf("  DPLL solver stats from last run:\n  ");
    sat_dpll_stats();
    printf("\n");
}

/* ── Benchmark 3: Reduction chain timing ───────────────────── */
static void bench_reduction_chain(void) {
    printf("── Benchmark 3: Reduction Chain Timing ──\n");
    printf("  Operation               | time_ms\n");
    printf("──────────────────────────┼─────────\n");

    /* Build a mid-size SAT formula */
    CNF* sat = cnf_create(10, 12);
    for (int ci = 0; ci < 12; ci++) {
        int k = (rand() % 3) + 2;
        int lits[4];
        for (int li = 0; li < k && li < 4; li++) {
            int v = rand() % 10;
            lits[li] = (rand() & 1) ? SAT_LIT_NEG(v) : SAT_LIT_POS(v);
        }
        cnf_add_clause(sat, lits, k);
    }
    printf("  SAT formula (10 vars, 12 clauses)  |      —\n");

    clock_t t0 = clock();
    CNF* sat3 = sat_to_3sat(sat);
    double ms_sat3 = ms_since(t0);
    printf("  SAT → 3SAT (%d vars, %d clauses)  | %7.3f\n",
           sat3->n_vars, sat3->n_clauses, ms_sat3);

    t0 = clock();
    Graph* g = sat3_to_clique(sat3);
    double ms_clique = ms_since(t0);
    printf("  3SAT → CLIQUE (%d vertices)      | %7.3f\n",
           g->n_vertices, ms_clique);

    t0 = clock();
    VCGraph* vcg = sat3_to_vertex_cover(sat3);
    double ms_vc = ms_since(t0);
    printf("  3SAT → VERTEX-COVER (%d vert,%d edges)| %7.3f\n",
           vcg->n_vertices, vcg->n_edges, ms_vc);

    printf("  Total chain time                  | %7.3f\n",
           ms_sat3 + ms_clique + ms_vc);

    cnf_free(sat);
    cnf_free(sat3);
    graph_free(g);
    vcgraph_free(vcg);
    printf("\n");
}

/* ── Main ──────────────────────────────────────────────────── */

int main(void) {
    srand(42);  /* deterministic for reproducibility */
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║   Complexity Scaling Benchmarks                         ║\n");
    printf("║   Cook-Levin Theorem Implementation                    ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    bench_cook_levin_scaling();
    bench_dpll_scaling();
    bench_reduction_chain();

    printf("═══ Benchmarks Complete ═══\n");
    return 0;
}
