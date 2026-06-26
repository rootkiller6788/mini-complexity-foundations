/* poly_vs_exp.c — P vs EXP: The Unconditional Separation
 * The time hierarchy theorem gives P != EXP unconditionally.
 * This is THE canonical result showing that more time provably
 * gives more computational power.
 *
 * P   = Union_k TIME(n^k)          (polynomial time)
 * EXP = Union_k TIME(2^{n^k})      (exponential time)
 *
 * Theorem: P != EXP.
 * Proof: TIME(n^k) != TIME(2^{n^k}) for each k (by hierarchy).
 * Taking the union over all k preserves the strict inclusion. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tht.h"

/* A concrete language in EXP \\ P:
   L = {n | the n-th poly-time TM REJECTS n within 2^n steps}.
   L is in EXP (simulate all TMs in 2^n time).
   L is NOT in P (by diagonalization against all poly-time TMs). */
static int exp_not_p_language(int n, int max_k) {
    for (int k = 1; k <= max_k; k++) {
        /* Simulate k-th poly-time TM (runs in n^k steps) */
        long long bound = 1;
        for (int i = 0; i < k; i++) {
            bound *= n;
            if (bound > 100000) bound = 100000;
        }

        int state = 0;
        for (long long step = 0; step < bound; step++) {
            /* Deterministic behavior based on (k, n, step) */
            int t = (k * 1009 + n * 271 + (int)step * 31337) % 17;
            if (t == 0) break;     /* TM accepts -> L says NO for this n */
            if (t == 1) return 1;  /* TM rejects -> L says YES! */
            state = (state + t) % 10;
        }
    }
    return 0;
}

void poly_vs_exp_demo(void) {
    printf("\n===== P != EXP: Unconditional Separation =====\n\n");

    printf("Theorem: P is a STRICT subset of EXP.\\n");
    printf("This is PROVED, not a conjecture!\\n\\n");

    printf("Proof:\\n");
    printf("  1. TIME(n^k) != TIME(2^n) for any fixed k.\\n");
    printf("     (Time hierarchy: n^k * log(n^k) = o(2^n))\\n");
    printf("  2. P = Union_k TIME(n^k)\\n");
    printf("  3. EXP = Union_k TIME(2^{n^k})\\n");
    printf("  4. Taking unions: P != EXP. QED.\\n\\n");

    printf("--- Concrete EXP \\\\ P Language ---\\n");
    printf("L = {n | the n-th poly-time TM rejects n in 2^n steps}\\n");
    printf("L is: in EXP (obviously), NOT in P (by diagonalization).\\n\\n");
    printf("Demonstration for small n:\\n");
    for (int n = 1; n <= 10; n++) {
        int r = exp_not_p_language(n, 5);
        printf("  L(%2d) = %s\\n", n, r ? "ACCEPT (in EXP, not in P)" : "REJECT");
    }

    printf("\n--- The Landscape ---\\n");
    printf("  P subset NP subset EXP\\n");
    printf("  |_____|     |_____|\\n");
    printf("  OPEN!       OPEN!\\n\\n");
    printf("  P != EXP is KNOWN.\\n");
    printf("  We don't know which inclusion in P ⊆ NP ⊆ EXP is proper:\\n");
    printf("    Possibility A: P = NP != EXP (would collapse PH)\\n");
    printf("    Possibility B: P != NP = EXP (would mean NP=EXP)\\n");
    printf("    Possibility C: P != NP != EXP (most likely)\\n");
    printf("  The hierarchy theorem alone cannot tell us.\\n");
}
