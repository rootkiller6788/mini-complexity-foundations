/* pspace_summary.c */
#include <stdio.h>
void pspace_summary_demo(void) {
    printf("\n===== PSPACE Module Summary =====\n\n");
    printf("Space Hierarchy: L in NL in P in NP in PSPACE=NPSPACE in EXP\n\n");
    printf("Key Theorems:\n");
    printf("  Savitch: PSPACE=NPSPACE. Immerman-Szelepcsenyi: NL=coNL.\n");
    printf("  CK-Stockmeyer: AP=PSPACE. TQBF is PSPACE-complete.\n\n");
    printf("Proven: L!=PSPACE, P!=EXP. Open: L vs NL, P vs PSPACE.\n\n");
    printf("PSPACE contains the ENTIRE polynomial hierarchy (PH).\n");
    printf("This makes it one of the largest natural complexity classes.\n");
}
