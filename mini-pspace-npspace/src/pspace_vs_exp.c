/* pspace_vs_exp.c — PSPACE vs EXP
 *
 * PSPACE ⊆ EXP is proven: a PSPACE machine has at most
 * 2^{poly(n)} configurations. Simulate all of them in
 * 2^{poly(n)} time = EXP.
 *
 * EXP ⊆ PSPACE is OPEN: we don't know if exponential time
 * can be simulated in polynomial space.
 *
 * The time hierarchy gives P != EXP, space hierarchy gives L != PSPACE.
 * But PSPACE vs EXP is a major open problem. */

#include <stdio.h>
#include <math.h>

/* Count configurations of a PSPACE machine:
   - n^k tape cells, each with |Gamma| possible symbols
   - n^k head positions, each with |Q| possible states
   - Total: |Gamma|^{n^k} * n^k * |Q| = 2^{O(n^k)} */
static double count_configs(int n, int k, int alphabet_size, int n_states) {
    double tape = pow(alphabet_size, pow(n, k));
    double position = pow(n, k);
    double total = tape * position * n_states;
    return total;
}

void pspace_vs_exp_demo(void) {
    printf("\n===== PSPACE vs EXP =====\n\n");

    printf("PSPACE = Union_k SPACE(n^k)\\n");
    printf("EXP    = Union_k TIME(2^{n^k})\\n\\n");

    printf("--- Known: PSPACE subset EXP ---\\n");
    printf("Proof: A PSPACE machine has at most 2^{O(n^k)} configs.\\n");
    printf("Simulate all configs in 2^{O(n^k)} time = EXP.\\n\\n");

    printf("--- Configuration Counting Demo ---\\n");
    printf("For n=5, k=2, |Gamma|=3, |Q|=5:\\n");
    double configs = count_configs(5, 2, 3, 5);
    printf("  Configs = 3^{5^2} * 5^2 * 5 = 3^25 * 125 ~ %.2e\\n", configs);
    printf("  This is HUGE but FINITE => can enumerate in EXP time.\\n\\n");

    printf("--- Open: EXP subset PSPACE? ---\\n");
    printf("Unknown! This is equivalent to: can exponential time\\n");
    printf("be simulated in polynomial space?\\n\\n");

    printf("If EXP ⊆ PSPACE, then PSPACE = EXP.\\n");
    printf("If EXP ⊈ PSPACE, then PSPACE ⊊ EXP (strict).\\n\\n");

    printf("--- The Landscape ---\\n");
    printf("  P ⊆ NP ⊆ PSPACE ⊆ EXP ⊆ NEXP ⊆ ...\\n");
    printf("  |_____|   |_______|   |_____|\\n");
    printf("  OPEN      OPEN         OPEN\\n");
    printf("  (but      (but         (but\\n");
    printf("   P!=EXP   L!=PSPACE    NEXP!=2EXP)\\n");
    printf("   proven)   proven)\\n\\n");
    printf("We know: P != EXP, L != PSPACE (proven by hierarchies).\\n");
    printf("We DON'T know: P vs NP, NP vs PSPACE, PSPACE vs EXP.\\n");
}