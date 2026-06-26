/*
 * example_reduction_chain.c — NP-Completeness Reduction Chain
 *
 * L4/L6: Demonstrates the chain of polynomial-time reductions:
 *   SAT → 3SAT → CLIQUE → VERTEX-COVER
 *
 * Each step demonstrates:
 *   1. Source instance construction
 *   2. Polynomial-time transformation to target
 *   3. Target problem solving
 *   4. Solution translation back
 *   5. Correctness verification
 *
 * This chain proves that 3SAT, CLIQUE, and VERTEX-COVER are NP-complete.
 *
 * Knowledge: Karp reductions, equisatisfiability, NPC certification,
 *            reduction transitivity, exact vs. heuristic solving.
 *
 * Reference: Karp (1972), Garey & Johnson (1979)
 * Courses: MIT 6.045, Stanford CS254, Berkeley CS172, CMU 15-855
 *
 * Build: gcc -I../include -o example_reduction_chain \
 *            example_reduction_chain.c ../src/reductions.c \
 *            ../src/sat_cnf.c ../src/dpll.c ../src/cook_levin.c \
 *            ../src/tableau.c ../src/turing_machine.c -lm
 */

#include "cook_levin.h"
#include "reduction.h"
#include "sat.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

/* ── Main ──────────────────────────────────────────────────── */

int main(void) {
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║   NP-Completeness Reduction Chain                      ║\n");
    printf("║   SAT → 3SAT → CLIQUE → VERTEX-COVER                  ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* ── Step 0: Create SAT instance ───────────────────────── */
    printf("═══ Step 0: Original SAT Formula ═══\n\n");
    printf("Building: φ = (x₁∨x₂) ∧ (¬x₁∨x₃) ∧ (¬x₂∨¬x₃) ∧ (x₁∨¬x₂∨¬x₄) ∧ (x₂∨x₃∨x₄)\n\n");
    CNF* sat_orig = cnf_create(4, 5);
    cnf_add_binary(sat_orig, SAT_LIT_POS(0), SAT_LIT_POS(1));
    cnf_add_binary(sat_orig, SAT_LIT_NEG(0), SAT_LIT_POS(2));
    cnf_add_binary(sat_orig, SAT_LIT_NEG(1), SAT_LIT_NEG(2));
    cnf_add_ternary(sat_orig, SAT_LIT_POS(0), SAT_LIT_NEG(1), SAT_LIT_NEG(3));
    cnf_add_ternary(sat_orig, SAT_LIT_POS(1), SAT_LIT_POS(2), SAT_LIT_POS(3));

    cnf_print_stats(sat_orig);
    Assignment sat_assign = assignment_create(4);
    int sat_ok = sat_solve_dpll(sat_orig, sat_assign);
    printf("SAT solve: %s\n", sat_ok ? "SAT" : "UNSAT");
    if (sat_ok) {
        printf("Assignment: x₁=%d x₂=%d x₃=%d x₄=%d\n",
               sat_assign[0], sat_assign[1], sat_assign[2], sat_assign[3]);
    }
    printf("#SAT count: %lld\n\n", sat_count_exact(sat_orig));

    /* ── Step 1: SAT → 3SAT ────────────────────────────────── */
    printf("═══ Step 1: SAT → 3SAT Reduction ═══\n\n");

    clock_t t0 = clock();
    CNF* f_3sat = sat_to_3sat(sat_orig);
    clock_t t1 = clock();

    printf("Original SAT: %d vars, %d clauses\n",
           sat_orig->n_vars, sat_orig->n_clauses);
    printf("3SAT result:  %d vars, %d clauses (max clause size=%d)\n",
           f_3sat->n_vars, f_3sat->n_clauses, cnf_max_clause_size(f_3sat));
    printf("Transformation time: %.6f ms\n",
           (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0);

    /* Equisatisfiability check */
    Assignment a_3sat = assignment_create(f_3sat->n_vars);
    int r_3sat = sat_solve_dpll(f_3sat, a_3sat);
    printf("3SAT solve: %s (equisatisfiable: %s)\n",
           r_3sat ? "SAT" : "UNSAT",
           (sat_ok == r_3sat) ? "✓ YES" : "✗ NO");
    printf("#SAT count: %lld\n\n", sat_count_exact(f_3sat));

    /* ── Step 2: 3SAT → CLIQUE ─────────────────────────────── */
    printf("═══ Step 2: 3SAT → CLIQUE Reduction ═══\n\n");

    t0 = clock();
    Graph* g_clique = sat3_to_clique(f_3sat);
    t1 = clock();

    int k_clique = f_3sat->n_clauses;
    printf("3SAT formula:   %d clauses\n", f_3sat->n_clauses);
    printf("CLIQUE graph:   %d vertices (1 per literal occurrence)\n",
           g_clique->n_vertices);
    printf("Looking for:    k=%d clique\n", k_clique);
    printf("Graph construction time: %.6f ms\n",
           (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0);

    /* Find clique */
    int* clique_v = (int*)calloc((size_t)k_clique, sizeof(int));
    t0 = clock();
    int found_clique = graph_has_k_clique(g_clique, k_clique, clique_v);
    t1 = clock();

    printf("Clique search:  %s (%.3f ms)\n",
           found_clique ? "FOUND" : "NOT FOUND",
           (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0);

    if (found_clique && k_clique <= 10) {
        printf("Clique vertices: ");
        for (int i = 0; i < k_clique; i++)
            printf("%d ", clique_v[i]);
        printf("\n");
        /* Verify each vertex is from a different clause */
        printf("Clause mapping:  ");
        for (int i = 0; i < k_clique; i++)
            printf("%d ", g_clique->vertex_to_clause[clique_v[i]]);
        printf("\n");
    }

    /* Translate clique back to 3SAT solution */
    if (found_clique) {
        int n_vars_3sat = f_3sat->n_vars;
        int* back_assign = (int*)calloc((size_t)n_vars_3sat, sizeof(int));
        for (int i = 0; i < n_vars_3sat; i++) back_assign[i] = -1;
        for (int i = 0; i < k_clique; i++) {
            int lit = g_clique->vertex_to_literal[clique_v[i]];
            int var = SAT_LIT_VAR(lit);
            int sign = SAT_LIT_SIGN(lit);
            if (var < n_vars_3sat)
                back_assign[var] = sign ? 0 : 1;
        }
        printf("Back-translated assignment satisfies 3SAT: %s\n",
               sat_verify(f_3sat, back_assign) ? "✓ YES" : "✗ NO");
        free(back_assign);
    }
    printf("\n");

    /* ── Step 3: 3SAT → VERTEX-COVER ───────────────────────── */
    printf("═══ Step 3: 3SAT → VERTEX-COVER Reduction ═══\n\n");

    t0 = clock();
    VCGraph* g_vc = sat3_to_vertex_cover(f_3sat);
    t1 = clock();

    int K_vc = f_3sat->n_vars + 2 * f_3sat->n_clauses;
    printf("VERTEX-COVER graph: %d vertices, %d edges\n",
           g_vc->n_vertices, g_vc->n_edges);
    printf("Target cover size:  K = n_vars + 2k = %d + 2×%d = %d\n",
           f_3sat->n_vars, f_3sat->n_clauses, K_vc);
    printf("Graph construction time: %.6f ms\n",
           (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0);

    int* cover_v = (int*)calloc((size_t)K_vc, sizeof(int));
    t0 = clock();
    int found_vc = vcgraph_has_vertex_cover(g_vc, K_vc, cover_v);
    t1 = clock();

    printf("Vertex cover:    %s (%.3f ms)\n",
           found_vc ? "FOUND" : "NOT FOUND",
           (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0);

    /* Verify the cover */
    if (found_vc) {
        int all_covered = 1;
        for (int e = 0; e < g_vc->n_edges; e++) {
            int covered = 0;
            for (int i = 0; i < K_vc; i++) {
                if (g_vc->edge_u[e] == cover_v[i] ||
                    g_vc->edge_v[e] == cover_v[i]) {
                    covered = 1;
                    break;
                }
            }
            if (!covered) { all_covered = 0; break; }
        }
        printf("Cover verification: %s (all %d edges covered)\n",
               all_covered ? "✓ VALID" : "✗ INVALID", g_vc->n_edges);
    }
    printf("\n");

    /* ── Summary ────────────────────────────────────────────── */
    printf("═══ Reduction Chain Summary ═══\n\n");
    printf("  SAT        (%d vars, %d clauses, %s)\n",
           sat_orig->n_vars, sat_orig->n_clauses, sat_ok ? "SAT" : "UNSAT");
    printf("    ↓ SAT → 3SAT (O(n²))\n");
    printf("  3SAT       (%d vars, %d clauses, %s)\n",
           f_3sat->n_vars, f_3sat->n_clauses, r_3sat ? "SAT" : "UNSAT");
    printf("    ↓ 3SAT → CLIQUE (O(n²))\n");
    printf("  CLIQUE     (%d vertices, k=%d, %s)\n",
           g_clique->n_vertices, k_clique, found_clique ? "Found" : "Not found");
    printf("    ↓ 3SAT → VERTEX-COVER (O(n²))\n");
    printf("  VC         (%d vertices, %d edges, K=%d, %s)\n",
           g_vc->n_vertices, g_vc->n_edges, K_vc, found_vc ? "Found" : "Not found");
    printf("\n");
    printf("All transformations are polynomial-time, establishing\n");
    printf("NP-completeness of 3SAT, CLIQUE, and VERTEX-COVER via\n");
    printf("the Karp reduction framework (Karp, 1972).\n");

    /* Cleanup */
    free(clique_v);
    free(cover_v);
    assignment_free(sat_assign);
    assignment_free(a_3sat);
    graph_free(g_clique);
    vcgraph_free(g_vc);
    cnf_free(sat_orig);
    cnf_free(f_3sat);

    printf("\n═══ Reduction Chain Demonstration Complete ═══\n");
    return 0;
}
