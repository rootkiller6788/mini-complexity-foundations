/* gi_demo.c -- Graph Isomorphism end-to-end example
 *
 * Demonstrates Graph Isomorphism as an NP-intermediate candidate.
 * Uses brute-force search, degree sequence invariant, and
 * Weisfeiler-Lehman color refinement.
 *
 * Build: make gi_demo
 * Run:   ./examples/gi_demo
 */

#include "ladner.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    setbuf(stdout, NULL);

    printf("\n========================================\n");
    printf("  Graph Isomorphism Demo: NPI Candidate\n");
    printf("========================================\n\n");

    printf("GI is in NP: guess permutation, verify in O(n^2).\n");
    printf("Babai (2015): quasipolynomial time!\n");
    printf("Not known in P, not known NPC (would collapse PH).\n\n");

    /* Build test graphs */
    printf("--- Test Graph 1: 4-Cycle ---\n");
    Graph* g1 = g_new(4);
    g_add_edge(g1, 0, 1); g_add_edge(g1, 1, 2);
    g_add_edge(g1, 2, 3); g_add_edge(g1, 3, 0);
    printf("  Edges: 0-1, 1-2, 2-3, 3-0\n\n");

    printf("--- Test Graph 2: Another 4-Cycle (different labeling) ---\n");
    Graph* g2 = g_new(4);
    g_add_edge(g2, 0, 2); g_add_edge(g2, 2, 1);
    g_add_edge(g2, 1, 3); g_add_edge(g2, 3, 0);
    printf("  Edges: 0-2, 2-1, 1-3, 3-0\n\n");

    printf("--- Test Graph 3: Star K_{1,3} ---\n");
    Graph* g3 = g_new(4);
    g_add_edge(g3, 0, 1); g_add_edge(g3, 0, 2);
    g_add_edge(g3, 0, 3);
    printf("  Edges: 0-1, 0-2, 0-3\n\n");

    /* Degree sequences */
    int d1[4], d2[4], d3[4];
    deg_seq(g1, d1); deg_seq(g2, d2); deg_seq(g3, d3);

    printf("--- Invariant 1: Degree Sequence ---\n");
    printf("  G1: ["); for(int i=0;i<4;i++) printf("%d ", d1[i]); printf("]\n");
    printf("  G2: ["); for(int i=0;i<4;i++) printf("%d ", d2[i]); printf("]\n");
    printf("  G3: ["); for(int i=0;i<4;i++) printf("%d ", d3[i]); printf("]\n");
    printf("  deg(G1) = deg(G2): %s (necessary condition)\n",
           deg_sequence_match(d1, d2, 4) ? "YES" : "NO");
    printf("  deg(G1) = deg(G3): %s\n\n",
           deg_sequence_match(d1, d3, 4) ? "YES" : "NO");

    /* WL test */
    printf("--- Invariant 2: Weisfeiler-Lehman (5 iterations) ---\n");
    printf("  WL(G1, G2): %s\n",
           wl_test(g1, g2, 5) > 0 ? "Cannot distinguish (possibly iso)" :
           "DISTINGUISHABLE (not iso)");
    printf("  WL(G1, G3): %s\n\n",
           wl_test(g1, g3, 5) > 0 ? "Cannot distinguish" :
           "DISTINGUISHABLE (not iso)");

    /* Brute force */
    printf("--- Ground Truth: Brute Force ---\n");
    printf("  G1 isomorphic to G2: %s\n",
           gi_brute(g1, g2) ? "YES" : "NO");
    printf("  G1 isomorphic to G3: %s\n\n",
           gi_brute(g1, g3) ? "YES" : "NO");

    /* Larger example: 5-cycle */
    printf("--- Larger Example: 5-Cycle ---\n");
    Graph* c5a = g_new(5);
    for (int i = 0; i < 5; i++) {
        g_add_edge(c5a, i, (i+1)%5);
    }
    /* Permute vertices */
    int pi[] = {2, 0, 4, 1, 3};
    Graph* c5b = g_permute(c5a, pi);
    printf("  Is 5-cycle isomorphic to permuted 5-cycle?\n");
    printf("  WL: %s\n",
           wl_test(c5a, c5b, 5) > 0 ? "possibly" : "NO");
    printf("  Brute: %s\n\n",
           gi_brute(c5a, c5b) ? "YES" : "NO");

    /* Algorithm comparison */
    printf("--- Algorithm Comparison ---\n");
    printf("  Brute force:  O(n!)    -- exact, impractical for n>10\n");
    printf("  WL refine:    O(n^3)   -- works for almost all graphs\n");
    printf("  Babai (2015): exp(O(log^c n)) -- QUASIPOLYNOMIAL\n\n");

    printf("--- Ladner Connection ---\n");
    printf("GI is the strongest candidate for a NATURAL\n");
    printf("NP-intermediate problem. Babai's quasipolynomial\n");
    printf("algorithm (2015) brings us tantalizingly close to\n");
    printf("polynomial time, suggesting GI might indeed be\n");
    printf("intermediate -- just as Ladner predicted in 1975.\n");

    g_free(c5b);
    g_free(c5a);
    g_free(g1);
    g_free(g2);
    g_free(g3);

    return 0;
}
