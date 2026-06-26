/*
 * example_cook_levin_reduction.c — Full Cook-Levin Theorem Walkthrough
 *
 * L4 Fundamental Law: Demonstrates the complete Cook-Levin reduction
 * from NTM acceptance to SAT formula, including:
 *   1. Building a Turing Machine for a specific NP language
 *   2. Running the NTM on inputs
 *   3. Performing the Cook-Levin tableau reduction
 *   4. Solving the resulting SAT formula
 *   5. Extracting and verifying the computation path
 *   6. Demonstrating both directions of the bi-implication
 *
 * Knowledge: TM, NTM, Cook-Levin tableau, variable encoding,
 *            clause groups G1-G4, path extraction, bi-implication.
 *
 * Reference: Cook (1971), Sipser §7.4, Arora-Barak §2.3
 * Courses: MIT 6.045/6.841, Stanford CS254, Berkeley CS172
 *
 * Build: gcc -I../include -o example_cook_levin_reduction \
 *            example_cook_levin_reduction.c ../src/cook_levin.c \
 *            ../src/tableau.c ../src/turing_machine.c \
 *            ../src/sat_cnf.c ../src/dpll.c -lm
 */

#include "cook_levin.h"
#include "turing_machine.h"
#include "sat.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

/* ── Main ──────────────────────────────────────────────────── */

int main(void) {
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Cook-Levin Theorem: SAT is NP-Complete               ║\n");
    printf("║  Full Reduction Walkthrough                            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* ── Part 1: Build an NTM for a simple NP language ─────── */
    printf("═══ Part 1: Turing Machine Construction ═══\n\n");

    printf("Building DTM that accepts { w | w contains 11 }:\n");
    TuringMachine* tm = tm_create_contains_11();
    assert(tm_validate(tm));
    tm_print_transitions(tm);

    printf("\n");
    Symbol input_accept[] = {0, 1, 1, 0};
    Symbol input_reject[] = {0, 1, 0, 1};

    printf("Testing TM on inputs:\n");
    int acc = tm_simulate_deterministic(tm, input_accept, 4, 100, NULL);
    int rej = tm_simulate_deterministic(tm, input_reject, 4, 100, NULL);
    printf("  w=\"0110\" → %s (expect ACCEPT)\n", acc == 1 ? "ACCEPT" : "REJECT");
    printf("  w=\"0101\" → %s (expect REJECT)\n\n", rej == 1 ? "ACCEPT" : "REJECT");

    /* ── Part 2: Cook-Levin Reduction ──────────────────────── */
    printf("═══ Part 2: Cook-Levin Tableau Reduction ═══\n\n");

    int T = 6;  /* time bound */
    printf("Reducing TM + input \"0110\" to SAT formula (T=%d)...\n", T);

    CLReduction* red_ac = cook_levin_reduce_detailed(tm, input_accept, 4, T);
    printf("  Total variables:  %d\n", red_ac->n_vars);
    printf("  Total clauses:    %d\n", red_ac->n_clauses);
    printf("  G1 (cell uniq):   %d\n", red_ac->n_g1);
    printf("  G2 (init+unique): %d\n", red_ac->n_g2);
    printf("  G3 (transitions): %d\n", red_ac->n_g3);
    printf("  G4 (acceptance):  %d\n", red_ac->n_g4);
    printf("  Generation time:  %.3f ms\n\n", red_ac->gen_time_ms);

    CNF* phi_accept = red_ac->formula;

    /* ── Part 3: Solve the SAT formula ─────────────────────── */
    printf("═══ Part 3: Solving the SAT Formula ═══\n\n");

    Assignment assign_ac = assignment_create(phi_accept->n_vars);
    clock_t t0 = clock();
    int sat_result = sat_solve_dpll(phi_accept, assign_ac);
    clock_t t1 = clock();

    printf("DPLL solver result: %s (%.3f ms)\n",
           sat_result ? "SAT" : "UNSAT",
           (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0);
    printf("SAT verification: %s\n\n",
           sat_verify(phi_accept, assign_ac) ? "VALID ✓" : "INVALID ✗");
    sat_dpll_stats();

    /* ── Part 4: Extract computation path ──────────────────── */
    printf("\n═══ Part 4: Path Extraction from SAT Assignment ═══\n\n");

    if (sat_result) {
        int P = T + 4 + 2;
        int n_syms = tm->tape_alpha_size > 0 ? tm->tape_alpha_size : 4;
        CLVariableScheme vs = cl_make_var_scheme(T, P, tm->n_states, n_syms);

        TMComputationPath* path = cook_levin_extract_path(
            &vs, assign_ac, tm, input_accept, 4, T);

        if (path) {
            printf("Extracted computation path (%d steps):\n", path->len - 1);
            tm_path_print(path);
            printf("Final state: %d (q_accept=%d)\n",
                   path->configs[path->len - 1]->state, tm->q_accept);
            tm_path_free(path);
        } else {
            printf("Warning: Could not extract valid path from SAT assignment.\n");
        }
    }

    /* ── Part 5: Verify bi-implication ─────────────────────── */
    printf("\n═══ Part 5: Bi-Implication Verification ═══\n\n");

    printf("Direction 1: M accepts w → φ is SAT\n");
    printf("  M accepts \"0110\": %s\n", acc == 1 ? "YES" : "NO");
    printf("  φ is SAT: %s\n", sat_result ? "YES" : "NO");
    printf("  Direction 1 holds: %s\n\n", (acc == 1) == (sat_result == 1) ? "✓" : "✗");

    printf("Direction 2: φ is SAT → M accepts w\n");
    CNF* phi_reject = cook_levin_reduce(tm, input_reject, 4, T);
    Assignment assign_rej = assignment_create(phi_reject->n_vars);
    int sat_rej = sat_solve_dpll(phi_reject, assign_rej);
    printf("  φ_reject is SAT: %s\n", sat_rej ? "YES" : "NO");
    printf("  M accepts \"0101\": %s\n", rej == 1 ? "YES" : "NO");
    printf("  Direction 2 holds: %s\n",
           (rej == 1) == (sat_rej == 1) ? "✓" : "✗");

    /* Cleanup */
    assignment_free(assign_ac);
    assignment_free(assign_rej);
    cnf_free(phi_reject);
    assignment_free(assign_rej);
    cl_reduction_free(red_ac);
    tm_free(tm);

    printf("\n═══ Cook-Levin Theorem Demonstration Complete ═══\n");
    printf("Summary: SAT is NP-complete — every NP language reduces to SAT.\n");
    printf("The tableau method provides a constructive, polynomial-time reduction.\n");
    return 0;
}
