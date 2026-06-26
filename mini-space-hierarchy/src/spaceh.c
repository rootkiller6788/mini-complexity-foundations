/* spaceh.c -- Space Hierarchy: Complete Overview
 * L = SPACE(log n) < NL < P < NP < PSPACE = NPSPACE < EXP
 * All strict inclusions except possibly the middle ones. */

#include "spaceh.h"

/* Measure space bounds for different classes */
static size_t space_log(int n) {
    int bits=0, t=n; while(t){bits++;t>>=1;}
    return (size_t)bits * sizeof(int);
}
static size_t space_lin(int n) { return (size_t)n * sizeof(int); }
static size_t space_quad(int n) { return (size_t)(n*n) * sizeof(int); }

/* space_hierarchy_diagonal_language defined in space_core.c */

void space_overview_demo(void) {
    printf("\n===== Space Complexity Hierarchy =====\n\n");
    printf("L* = SPACE(log n)     [* = provably different from PSPACE]\n");
    printf("NL = NSPACE(log n)    [NL=coNL PROVED (Immerman-Szelepcsenyi)]\n");
    printf("P  = TIME(poly)\n");
    printf("NP = NTIME(poly)\n");
    printf("PSPACE* = SPACE(poly) = NPSPACE   [Savitch collapse]\n");
    printf("EXP* = TIME(2^poly)   [P!=EXP PROVED]\n\n");

    printf("--- Space Usage Comparison ---\n");
    printf("%8s %12s %12s %12s\n","n","O(log n)","O(n)","O(n^2)");
    for(int n=10;n<=10000;n*=10)
        printf("%8d %10zu B %10zu B %10zu KB\n",n,
               space_log(n),space_lin(n),space_quad(n)/1024);

    printf("\n--- Diagonal Language Demo ---\n");
    printf("L_D = {n | M_n rejects within n^2 space, uses <= n space}\n");
    for(int n=0;n<=12;n++)
        printf("  L_D(%2d) = %d\n",n,space_hierarchy_diagonal_language(n));

    printf("\n--- Key Open Problems ---\n");
    printf("L vs NL: nondeterminism at logspace level?\n");
    printf("NL vs P: Can PATH be solved in P without logspace?\n");
    printf("P vs PSPACE: poly time vs poly space?\n");
    printf("PSPACE vs EXP: space vs time at exponential scale?\n");
}
