/* reduction_demo.c — demonstrate SAT -> 3SAT -> CLIQUE reduction */
#include "types.h"
#include "sat.h"
#include "reduction.h"
#include <stdio.h>

int main(void) {
    printf("===== NP Reduction Demo =====\n\n");
    CNF* f = cnf_new(4, 2);
    int c1[] = {(0<<1)|0,(1<<1)|0,(2<<1)|0,(3<<1)|0}; /* 4-clause */
    int c2[] = {(0<<1)|1,(1<<1)|1};
    cnf_add(f,c1,4); cnf_add(f,c2,2);
    printf("Original (4SAT):\n"); cnf_print(f);

    /* SAT -> 3SAT */
    CNF* f3 = reduce_sat_to_3sat(f);
    printf("\nReduced to 3SAT (%d vars,%d clauses):\n", f3->n_vars,f3->n_clauses);
    cnf_print(f3);

    /* Verify reduction */
    int r = reduction_verify(f, f3);
    printf("\nReduction verified: %s\n", r?"PASS":"FAIL");

    cnf_free(f); cnf_free(f3);
    return 0;
}
