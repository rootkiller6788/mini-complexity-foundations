/* alternation.c -- Alternating Time Hierarchy
 * Chandra-Kozen-Stockmeyer (1981):
 * ATIME(f, k) strict subset ATIME(g, k) for appropriate f,g.
 * AP = ATIME(poly, unbounded) = PSPACE. */

#include <stdio.h>
#include "tht.h"

static int atime_simulate(int depth, int k, int is_exist, int tm_id, int input) {
    if (depth >= k) {
        return ((tm_id * 31 + input * 17 + depth * 7) % 3) != 0;
    }
    int c0 = atime_simulate(depth + 1, k, !is_exist, tm_id, input * 2);
    int c1 = atime_simulate(depth + 1, k, !is_exist, tm_id, input * 2 + 1);
    return is_exist ? (c0 || c1) : (c0 && c1);
}

void alternation_demo(void) {
    printf("\n===== Alternating Time Hierarchy =====\n\n");
    printf("Chandra-Kozen-Stockmeyer (1981):\n");
    printf("  ATIME(poly, k alts, start EXISTS) = Sigma_k\n");
    printf("  ATIME(poly, k alts, start FORALL) = Pi_k\n");
    printf("  ATIME(poly, unbounded) = PSPACE\n");
    printf("  ALOGSPACE = P\n\n");

    printf("--- ATIME Simulation Demo ---\n");
    printf("ATIME(k alternations) for different k:\n\n");
    for (int k = 0; k <= 4; k++) {
        int accept_count = 0;
        for (int inp = 0; inp < 20; inp++) {
            if (atime_simulate(0, k, 1, 0, inp)) accept_count++;
        }
        printf("  k=%d: %d/20 inputs accepted\n", k, accept_count);
    }

    printf("\n--- Hierarchy ---\n");
    printf("ATIME(poly,0) = P\n");
    printf("ATIME(poly,1,EXISTS) = NP\n");
    printf("ATIME(poly,2,EXISTS) = Sigma_2\n");
    printf("ATIME(poly,unbounded) = PSPACE\n\n");

    printf("Key insight: Each alternation adds power,\n");
    printf("up to PSPACE where unbounded alternation lives.\n");
}
