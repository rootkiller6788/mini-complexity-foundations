/* logspace_reductions.c */
#include <stdio.h>
void logspace_reductions_demo(void) {
    printf("\n===== Logspace Reductions for PSPACE =====\n\n");
    printf("PSPACE-completeness uses <=_L (logspace) reductions.\n\n");
    printf("SAT -> TQBF: add quantifier prefix (O(log n) space).\n");
    printf("TQBF -> Geography: encode QBF as game position.\n\n");
    printf("Space analysis: input/output on separate tapes.\n");
    printf("Only WORK tape counts: O(log n) for indices/counters.\n\n");
    printf("All PSPACE-complete problems equivalent under <=_L.\n");
    printf("If any in L, then L=PSPACE (extremely unlikely).\n");
}
