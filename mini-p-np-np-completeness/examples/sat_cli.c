/* sat_cli.c — command-line SAT solver (DIMACS in, solution out) */
#include "types.h"
#include "sat.h"
#include "dimacs.h"
#include <stdio.h>
#include <time.h>

int main(int argc, char** argv) {
    if (argc < 2) { fprintf(stderr,"Usage: sat_cli <file.cnf>\n"); return 1; }
    CNF* cnf = dimacs_parse(argv[1]);
    if (!cnf) { fprintf(stderr,"Failed to parse %s\n",argv[1]); return 1; }
    printf("Parsed: %d vars, %d clauses\n", cnf->n_vars, cnf->n_clauses);
    int* assign = malloc((size_t)cnf->n_vars * sizeof(int));
    clock_t t0 = clock();
    int result = sat_solve(cnf, assign);
    double t = (double)(clock()-t0)/CLOCKS_PER_SEC;
    printf("Result: %s (%.3fs)\n", result?"SATISFIABLE":"UNSATISFIABLE", t);
    if (result) {
        printf("Assignment: ");
        for (int i=0; i<cnf->n_vars && i<20; i++) printf("x%d=%d ",i+1,assign[i]);
        if (cnf->n_vars>20) printf("...");
        printf("\n");
    }
    free(assign); cnf_free(cnf);
    return result ? 10 : 20; /* SAT Competition convention */
}
