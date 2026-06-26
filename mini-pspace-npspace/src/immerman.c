/* immerman.c — Immerman-Szelepcsenyi Theorem (1987)
 * NL = coNL. Won Godel Prize 1995.
 *
 * Proof: inductive counting. To verify non-reachability in a graph,
 * count how many vertices ARE reachable. If target is not among them,
 * then it's unreachable.
 *
 * This shows nondeterministic logspace is closed under complement. */
#include <stdio.h>

void immerman_demo(void) {
    printf("\n===== Immerman-Szelepcsenyi Theorem =====\n\n");
    printf("NL = coNL (Immerman 1987, Szelepcsenyi 1987)\n\n");
    printf("Proof technique: inductive counting.\n");
    printf("  1. To verify s -/-> t in digraph:\n");
    printf("  2. Count R_k = #vertices reachable from s in <= k steps\n");
    printf("  3. For each vertex v, nondeterministically guess if v in R_k\n");
    printf("  4. Verify guessed count matches actual count\n");
    printf("  5. If t not in R_k, s does not reach t\n\n");
    printf("Key insight: counting + verification can be done in logspace.\n");
    printf("This is non-obvious because you cannot store all reachable vertices.\n");
    printf("The inductive counting technique circumvents this.\n\n");
    printf("Consequence: the complement of any NL problem is also in NL.\n");
    printf("This is fundamentally different from NP vs coNP (unknown).\n");
}