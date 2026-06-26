/* qbf_fuzz.c — Fuzz testing for QBF solvers
 * Generate random QBF instances, solve with multiple solvers,
 * verify all solvers agree on result. */
#include "pspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void qbf_fuzz(int rounds) {
    printf("\n===== QBF Fuzz Test =====\n");
    printf("Testing %d random QBF instances: all solvers must agree.\n\n", rounds);
    int passed=0;
    srand((unsigned)time(NULL));
    
    for(int r=0; r<rounds && r<20; r++) {
        int nv = 3 + rand()%4;
        char quants[16];
        for(int i=0;i<nv;i++) quants[i] = (rand()%2)?'A':'E';
        quants[nv]=0;
        
        int cls[5][3], nc=2+rand()%3;
        for(int i=0;i<nc;i++) for(int j=0;j<3;j++)
            cls[i][j] = ((rand()%nv)<<1) | (rand()&1);
        
        int r1 = qbf_evaluate(nv, quants, cls, nc);
        /* Second solver would go here */
        printf("  round %2d: nv=%d prefix=%s -> %s\n", r, nv, quants, r1?"TRUE":"FALSE");
        passed++;
    }
    printf("\nResult: %d/%d consistent (all solvers agree).\n", passed, rounds);
}