/*
 * example_verifier_framework.c — NP Certificate/Verifier Framework
 *
 * L2/L7: Demonstrates the certificate definition of NP:
 *   L ∈ NP iff ∃ poly-time verifier V such that
 *   x ∈ L ⇔ ∃ certificate y with |y| ≤ poly(|x|) and V(x,y) = 1
 *
 * This example shows:
 *   1. SAT verifier (polynomial time)
 *   2. CLIQUE verifier
 *   3. VERTEX-COVER verifier
 *   4. Solve vs. Verify timing comparison
 *   5. Invalid certificate detection
 *   6. Certificate length bound checking
 *
 * Knowledge: NP certificate definition, polynomial-time verification,
 *            P vs NP gap, solve/verify asymmetry, witness.
 *
 * Reference: Sipser §7.3, Arora-Barak §2.1
 * Courses: MIT 6.045/6.841, Stanford CS254, Berkeley CS172, CMU 15-855
 *
 * Build: gcc -I../include -o example_verifier_framework \
 *            example_verifier_framework.c ../src/verifier.c \
 *            ../src/cook_levin.c ../src/tableau.c \
 *            ../src/turing_machine.c ../src/sat_cnf.c \
 *            ../src/dpll.c ../src/reductions.c -lm
 */

#include "cook_levin.h"
#include "reduction.h"
#include "sat.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/*
 * CLIQUE verifier: Given a graph G and a claimed clique of size k,
 * verify in O(k²) time that all pairs in the clique are adjacent.
 */
static int verify_clique_certificate(const Graph* g, int k, const int* clique) {
    if (!g || !clique || k <= 0) return 0;
    for (int i = 0; i < k; i++) {
        if (clique[i] < 0 || clique[i] >= g->n_vertices) return 0;
        for (int j = i + 1; j < k; j++) {
            if (clique[j] < 0 || clique[j] >= g->n_vertices) return 0;
            if (!g->adj_matrix[clique[i] * g->n_vertices + clique[j]])
                return 0;
        }
    }
    return 1;
}

/*
 * VERTEX-COVER verifier: Given a graph G and a claimed cover of size K,
 * verify in O(K ⋅ |E|) time that every edge is covered.
 */
static int verify_vertexcover_certificate(const VCGraph* g, int K,
                                           const int* cover) {
    if (!g || !cover || K <= 0) return 0;
    for (int e = 0; e < g->n_edges; e++) {
        int covered = 0;
        for (int i = 0; i < K; i++) {
            if (cover[i] < 0 || cover[i] >= g->n_vertices) continue;
            if (g->edge_u[e] == cover[i] || g->edge_v[e] == cover[i]) {
                covered = 1;
                break;
            }
        }
        if (!covered) return 0;
    }
    return 1;
}

/* ── Main ──────────────────────────────────────────────────── */

int main(void) {
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║   NP Certificate / Verifier Framework                  ║\n");
    printf("║   \"Easy to verify, (possibly) hard to solve\"           ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* ── Part 1: SAT Verification ──────────────────────────── */
    printf("═══ Part 1: SAT — Polynomial-Time Verification ═══\n\n");

    printf("Building CNF: (x₁∨x₂) ∧ (¬x₁∨x₃) ∧ (¬x₂∨¬x₃)\n");
    CNF* cnf = cnf_create(3, 3);
    cnf_add_binary(cnf, SAT_LIT_POS(0), SAT_LIT_POS(1));
    cnf_add_binary(cnf, SAT_LIT_NEG(0), SAT_LIT_POS(2));
    cnf_add_binary(cnf, SAT_LIT_NEG(1), SAT_LIT_NEG(2));

    /* Solve (exponential worst-case) */
    clock_t t0 = clock();
    Assignment assign = assignment_create(3);
    int sat_res = sat_solve_dpll(cnf, assign);
    clock_t t1 = clock();
    double solve_ms = (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0;

    printf("Solve (DPLL):     %s (%.6f ms)\n",
           sat_res ? "SAT" : "UNSAT", solve_ms);

    /* Verify (polynomial time) */
    t0 = clock();
    int verify_res = sat_verify(cnf, assign);
    t1 = clock();
    double verify_ms = (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0;

    printf("Verify (O(|φ|)):  %s (%.9f ms)\n",
           verify_res ? "VALID" : "INVALID", verify_ms);
    printf("Solve/Verify ratio: %.1f×\n\n",
           verify_ms > 0 ? solve_ms / verify_ms : 0.0);

    /* Verify with invalid certificate */
    int orig_x1 = assign[0];
    assign[0] = 1 - orig_x1;  /* flip x1 to create invalid assignment */
    printf("With WRONG certificate (flip x₁): verify = %s\n",
           sat_verify(cnf, assign) ? "VALID" : "INVALID");
    assign[0] = orig_x1;  /* restore */
    printf("With CORRECT certificate:         verify = %s\n\n",
           sat_verify(cnf, assign) ? "VALID" : "INVALID");

    /* ── Part 2: CLIQUE Verification ────────────────────────── */
    printf("═══ Part 2: CLIQUE — Certificate Verification ═══\n\n");

    /* Build a specific graph where we know there's a 4-clique */
    int n_v = 6;
    Graph* g = (Graph*)calloc(1, sizeof(Graph));
    g->n_vertices = n_v;
    g->adj_matrix = (int*)calloc((size_t)n_v * n_v, sizeof(int));
    g->vertex_to_literal = (int*)calloc((size_t)n_v, sizeof(int));
    g->vertex_to_clause = (int*)calloc((size_t)n_v, sizeof(int));

    /* Make vertices 0,1,2,3 form a 4-clique, plus extra vertices 4,5 */
    for (int i = 0; i < 4; i++)
        for (int j = i + 1; j < 4; j++) {
            g->adj_matrix[i * n_v + j] = 1;
            g->adj_matrix[j * n_v + i] = 1;
        }
    /* Vertex 4 connected to 0,1; vertex 5 connected to 2,3 */
    g->adj_matrix[4 * n_v + 0] = g->adj_matrix[0 * n_v + 4] = 1;
    g->adj_matrix[4 * n_v + 1] = g->adj_matrix[1 * n_v + 4] = 1;
    g->adj_matrix[5 * n_v + 2] = g->adj_matrix[2 * n_v + 5] = 1;
    g->adj_matrix[5 * n_v + 3] = g->adj_matrix[3 * n_v + 5] = 1;

    /* Valid k=4 clique certificate: vertices {0,1,2,3} */
    int valid_4clique[] = {0, 1, 2, 3};
    printf("Graph: %d vertices, known 4-clique {0,1,2,3}\n", n_v);
    printf("Verify 4-clique {0,1,2,3}: %s (expect VALID)\n",
           verify_clique_certificate(g, 4, valid_4clique) ? "VALID" : "INVALID");

    /* Invalid: {0,4,5,?} — 4 and 5 are not adjacent */
    int invalid_4clique[] = {0, 4, 5, 1};
    printf("Verify 4-clique {0,4,5,1}: %s (expect INVALID, 4-5 not edge)\n",
           verify_clique_certificate(g, 4, invalid_4clique) ? "VALID" : "INVALID");

    /* k=3 clique {0,1,4} should be valid */
    int valid_3clique[] = {0, 1, 4};
    printf("Verify 3-clique {0,1,4}: %s (expect VALID)\n",
           verify_clique_certificate(g, 3, valid_3clique) ? "VALID" : "INVALID");

    /* Verify certificate length bound: asking for k=10 but only have 4 items */
    printf("Verify k=10 but only 4 vertices: %s (expect INVALID)\n\n",
           verify_clique_certificate(g, 10, valid_4clique) ? "VALID" : "INVALID");

    /* ── Part 3: VERTEX-COVER Verification ──────────────────── */
    printf("═══ Part 3: VERTEX-COVER — Certificate Verification ═══\n\n");

    /* Build a triangle graph (3 vertices, 3 edges) */
    VCGraph* vcg = (VCGraph*)calloc(1, sizeof(VCGraph));
    vcg->n_vertices = 3;
    vcg->n_edges = 3;
    vcg->adj_matrix = (int*)calloc(9, sizeof(int));
    vcg->edge_u = (int*)malloc(3 * sizeof(int));
    vcg->edge_v = (int*)malloc(3 * sizeof(int));
    vcg->adj_matrix[0 * 3 + 1] = vcg->adj_matrix[1 * 3 + 0] = 1;
    vcg->adj_matrix[1 * 3 + 2] = vcg->adj_matrix[2 * 3 + 1] = 1;
    vcg->adj_matrix[2 * 3 + 0] = vcg->adj_matrix[0 * 3 + 2] = 1;
    vcg->edge_u[0] = 0; vcg->edge_v[0] = 1;
    vcg->edge_u[1] = 1; vcg->edge_v[1] = 2;
    vcg->edge_u[2] = 2; vcg->edge_v[2] = 0;

    /* Minimum vertex cover of triangle is 2 vertices */
    int cover_2[] = {0, 1};  /* covers edges (0,1), (1,2), (0,2) */
    printf("Triangle graph: 3 vertices, 3 edges, min cover size = 2\n");
    printf("Verify cover {0,1} (K=2): %s (expect VALID)\n",
           verify_vertexcover_certificate(vcg, 2, cover_2) ? "VALID" : "INVALID");

    /* K=1 cover impossible for triangle */
    int cover_1[] = {0};
    printf("Verify cover {0} (K=1):   %s (expect INVALID)\n",
           verify_vertexcover_certificate(vcg, 1, cover_1) ? "VALID" : "INVALID");
    printf("\n");

    /* ── Part 4: Certificate Length Bound ───────────────────── */
    printf("═══ Part 4: Certificate Length Bound ═══\n\n");

    int cert_bound = 10;  /* poly(|x|) bound */
    printf("For a CNF of size |φ|=%d literals:\n", cnf_total_literals(cnf));
    printf("  Certificate bound: |y| ≤ %d\n", cert_bound);
    printf("  SAT assignment has %d variables (within bound: %s)\n",
           cnf->n_vars, (cnf->n_vars <= cert_bound) ? "YES" : "NO");
    printf("\n");

    /* ── Part 5: NP Class Summary ───────────────────────────── */
    printf("═══ Part 5: NP Class Characteristics ═══\n\n");
    printf("Definition: L ∈ NP iff ∃ poly-time verifier V and\n");
    printf("  polynomial p such that for all strings x:\n");
    printf("    x ∈ L ⇔ ∃ certificate y, |y| ≤ p(|x|), V(x,y)=1\n\n");
    printf("Observed properties:\n");
    printf("  1. Verification is FAST (polynomial time in |x|)\n");
    printf("  2. Finding a certificate can be SLOW (exponential)\n");
    printf("  3. An invalid certificate is quickly rejected\n");
    printf("  4. The certificate must be bounded by poly(|x|)\n");
    printf("  5. SAT ∈ NP (verifier checks assignment in O(|φ|))\n");
    printf("  6. CLIQUE ∈ NP (verifier checks pairs in O(k²))\n");
    printf("  7. VERTEX-COVER ∈ NP (verifier checks edges in O(K·|E|))\n");
    printf("  8. Cook-Levin: SAT is NP-complete (every L∈NP ≤_p SAT)\n");

    /* Cleanup */
    assignment_free(assign);
    cnf_free(cnf);
    free(g->adj_matrix);
    free(g->vertex_to_literal);
    free(g->vertex_to_clause);
    free(g);
    free(vcg->adj_matrix);
    free(vcg->edge_u);
    free(vcg->edge_v);
    free(vcg);

    printf("\n═══ NP Verifier Framework Demonstration Complete ═══\n");
    return 0;
}
