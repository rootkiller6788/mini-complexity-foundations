/* qbf_to_sat.c — Reduce QBF to SAT for bounded quantifier depth
 * For Sigma_k or Pi_k formulas, expand outermost quantifiers
 * to produce a purely existential (SAT) formula.
 * Sigma_2: exists x forall y: phi(x,y) -> SAT by expanding forall. */
#include "pspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Expand a QBF prefix to SAT by grounding quantifiers.
 * For exists x forall y: phi -> exists x: (phi[y:=0] AND phi[y:=1])
 * Continue recursively. Exponentially blows up formula size. */
int qbf_to_sat_reduce(int n, const char* quants, int cls[][3], int nc,
                      int*** out_clauses, int* out_nc) {
    /* Count number of existential vars after the first universal block */
    int exists_block_end = 0;
    while(exists_block_end < n && quants[exists_block_end] == 'E') exists_block_end++;
    if(exists_block_end == n) {
        /* All existential = SAT. Copy clauses directly. */
        *out_nc = nc;
        *out_clauses = malloc((size_t)nc * sizeof(int*));
        for(int i=0;i<nc;i++) {
            (*out_clauses)[i] = malloc((size_t)3 * sizeof(int));
            memcpy((*out_clauses)[i], cls[i], 3 * sizeof(int));
        }
        return 1;
    }

    /* Has universal quantifiers: expand first universal block */
    int univ_block_end = exists_block_end;
    while(univ_block_end < n && quants[univ_block_end] == 'A') univ_block_end++;
    int n_univ = univ_block_end - exists_block_end;

    /* For each universal: need both 0 and 1 assignments -> 2^{n_univ} copies */
    int copies = 1 << n_univ;
    if(copies > 64) { printf("  Too many universals to expand (%d vars)\n", n_univ); return 0; }

    /* Create 2^{n_univ} copies of each clause with universals grounded */
    *out_nc = nc * copies;
    *out_clauses = malloc((size_t)(*out_nc) * sizeof(int*));

    int clause_idx = 0;
    for(int ci=0; ci<nc; ci++) {
        for(int mask=0; mask<copies; mask++) {
            int* new_cl = malloc((size_t)3 * sizeof(int));
            int nl=0;
            for(int j=0;j<3;j++) {
                int lit = cls[ci][j], var = lit>>1, sign = lit&1;
                if(var >= exists_block_end && var < univ_block_end) {
                    /* This universal is grounded by mask */
                    int uidx = var - exists_block_end;
                    int val = (mask>>uidx)&1;
                    /* If literal is satisfied by val, skip clause (make TRUE) */
                    if((!sign && val) || (sign && !val)) { nl=-1; break; }
                    /* Otherwise skip this literal (it's FALSE) */
                } else {
                    new_cl[nl++] = lit;
                }
            }
            if(nl >= 0) {
                (*out_clauses)[clause_idx] = new_cl;
                /* pad with -1 for unused slots */
                while(nl<3) new_cl[nl++] = -1;
            } else {
                /* Clause trivially satisfied => make it a tautology */
                new_cl[0]=0;new_cl[1]=1;new_cl[2]=-1;
                (*out_clauses)[clause_idx] = new_cl;
            }
            clause_idx++;
        }
    }
    (void)clause_idx;
    return 1;
}

void qbf_to_sat_demo(void) {
    printf("\n===== QBF to SAT Reduction =====\n\n");
    printf("For fixed quantifier depth k, QBF reduces to SAT with\n");
    printf("blowup factor 2^{n_universal}.\n");
    printf("Sigma_2: exists x forall y: phi -> exists x: phi[y:=0] AND phi[y:=1]\n");
    printf("After expansion: purely existential = SAT instance.\n\n");
    
    printf("Example: E x0 A x1: (x0|x1)&(!x0|!x1)\n");
    printf("Expand forall x1 -> ground to 0 and 1:\n");
    printf("  E x0: ((x0|0)&(!x0|!0)) AND ((x0|1)&(!x0|!1))\n");
    printf("  = E x0: (x0 & !x0) AND (TRUE & !x0)\n");
    printf("  = UNSAT\n");
    printf("This is exponential-space but works for small universal blocks.\n");
}