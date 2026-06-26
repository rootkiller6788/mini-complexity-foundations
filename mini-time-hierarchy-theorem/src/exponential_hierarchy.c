/* exponential_hierarchy.c — The Exponential Hierarchy
 *
 * Above EXP lies an infinite tower of provably distinct classes:
 * EXP != 2-EXP != 3-EXP != ... != k-EXP for all k.
 *
 * The time hierarchy extends upward indefinitely.
 * Unlike PH (which might collapse), the EXPONENTIAL hierarchy
 * is PROVABLY strict at every level.
 *
 * ELEMENTARY = Union_k k-EXP (all finite towers of 2^).
 * PR = Primitive Recursive (includes Ackermann-like functions).
 * RECURSIVE = All decidable languages (strictly larger than PR). */

#include <stdio.h>
#include <math.h>
#include "tht.h"

/* Compute tower of exponentials: 2^(2^(...^2^(n))) with k levels.
   Level 0: n. Level 1: 2^n. Level 2: 2^{2^n}. etc. */
static double exp_tower(double n, int levels) {
    double result = n;
    for (int i = 0; i < levels; i++) {
        result = pow(2.0, result);
        if (result > 1e100) return 1e100;
    }
    return result;
}

void exponential_hierarchy_demo(void) {
    printf("\n===== The Exponential Hierarchy =====\n\n");

    printf("Above EXP lies an INFINITE tower of provably distinct classes:\\n\\n");

    printf("  Class       Definition                    n=2 value\\n");
    printf("  ----------  ---------------------------   ---------\\n");
    printf("  P           Union_k TIME(n^k)               4\\n");
    printf("  EXP         Union_k TIME(2^{n^k})           4\\n");
    printf("  2-EXP       Union_k TIME(2^{2^{n^k}})       2^4 = 16\\n");
    printf("  3-EXP       TIME(2^{2^{2^{n^k}}})           2^16 = 65536\\n");
    printf("  4-EXP       ...                             2^65536 ~ 10^19728\\n");
    printf("  ...\\n");
    printf("  ELEMENTARY  Union over all k-EXP           finite towers\\n");
    printf("  PR          Primitive Recursive            Ackermann(2,2)\\n");
    printf("  RECURSIVE   All decidable languages        unbounded\\n\\n");

    printf("--- Tower Growth Demonstration ---\\n");
    for (int n = 0; n <= 4; n++) {
        printf("  n=%d: ", n);
        for (int k = 0; k <= 3; k++) {
            double val = exp_tower((double)n, k);
            if (val < 1e8)
                printf("k=%d: %.0f  ", k, val);
            else
                printf("k=%d: HUGE  ", k);
        }
        printf("\\n");
    }

    printf("\n--- Key Theorem ---\\n");
    printf("For every k: k-EXP != (k+1)-EXP (PROVED!).\\n");
    printf("The exponential hierarchy CANNOT collapse.\\n");
    printf("This is in STARK CONTRAST to PH which MIGHT collapse.\\n\\n");

    printf("--- Relationship to PH ---\\n");
    printf("PH ⊆ PSPACE ⊆ EXP ⊆ 2-EXP ⊆ ...\\n");
    printf("If PH is infinite (does not collapse), it is contained\\n");
    printf("within PSPACE, which is contained within EXP.\\n");
    printf("Thus the exponential hierarchy is MUCH larger than PH.\\n\\n");

    printf("--- Beyond ELEMENTARY ---\\n");
    printf("PR (Primitive Recursive) contains Ackermann's function.\\n");
    printf("RECURSIVE contains the halting problem (undecidable).\\n");
    printf("There are still undecidable problems above RECURSIVE\\n");
    printf("(by the arithmetical hierarchy and beyond).\\n");
}
