/* space_classes.c — Space Complexity Class Hierarchy
 *
 * L = SPACE(log n)
 * NL = NSPACE(log n)  
 * P = polynomial time
 * PSPACE = polynomial space = NPSPACE (by Savitch)
 * EXP = exponential time
 *
 * Known: L subset NL subset P subset NP subset PSPACE subset EXP
 * Open: L vs NL? L vs P? P vs PSPACE? */
#include <stdio.h>

void space_complexity_summary(void) {
    printf("\n===== Space Complexity Hierarchy =====\n\n");
    printf("L       = SPACE(log n)     — logspace computation\n");
    printf("NL      = NSPACE(log n)    — nondeterministic logspace\n");
    printf("P       = TIME(poly(n))    — polynomial time\n");
    printf("NP      = NTIME(poly(n))   — nondeterministic poly time\n");
    printf("PSPACE  = SPACE(poly(n))   — polynomial space\n");
    printf("NPSPACE = NSPACE(poly(n)) = PSPACE (Savitch)\n");
    printf("EXP     = TIME(2^{poly})   — exponential time\n\n");
    
    printf("Known relationships:\n");
    printf("  L subset NL subset P subset NP subset PSPACE subset EXP\n");
    printf("  NL = coNL (Immerman-Szelepcsenyi 1987)\n");
    printf("  PSPACE = NPSPACE (Savitch 1970)\n\n");
    
    printf("Major open problems:\n");
    printf("  L vs NL ?\n");
    printf("  NL vs P ?\n");
    printf("  P vs PSPACE ?\n");
    printf("  PSPACE vs EXP ?\n\n");
    
    printf("Uniform diagonalization CAN separate:\n");
    printf("  SPACE(f) subsetneq SPACE(g) when f=o(g)\n");
    printf("  => L subsetneq PSPACE (unconditional!)\n");
}