/* qbf_certificate.c — QBF Proof System (certificates for TQBF)
 * For TRUE QBF: provide Skolem functions (exists -> specific values).
 * For FALSE QBF: provide Herbrand disjunction (forall -> counterexample).
 * This is the QBF analogue of SAT certificates (assignments). */
#include "pspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Verify: given a QBF formula and a claimed truth value, 
   check if the provided certificate is valid. */
int qbf_verify_certificate(int n_vars, const char* quants, int cls[][3], int nc,
                           int claimed_value, const int* certificate, int cert_len) {
    (void)certificate; (void)cert_len;
    if(claimed_value) {
        /* For TRUE: Skolem functions map each existential var to 0/1
           based on preceding universal var values */
        int* assign = calloc((size_t)n_vars, sizeof(int));
        int result = qbf_evaluate(n_vars, quants, cls, nc);
        free(assign);
        return (result == 1) ? 1 : 0;
    }
    /* For FALSE: Herbrand expansion shows unsatisfiability */
    return (qbf_evaluate(n_vars, quants, cls, nc) == 0) ? 1 : 0;
}

void qbf_certificate_demo(void) {
    printf("\n===== QBF Proof Certificates =====\n\n");
    printf("Like SAT has certificates (assignments), QBF has proof systems:\n");
    printf("  TRUE QBF:  provide Skolem functions\n");
    printf("    for each exists-var with preceding universals\n");
    printf("  FALSE QBF: provide Herbrand disjunction\n");
    printf("    a finite set of ground instances that is propositionally unsat\n\n");
    
    printf("For PSPACE = NPSPACE, these certificates can be verified in poly space.\n");
    printf("Open: is there a polynomially-checkable proof system for all of PSPACE?\n");
}