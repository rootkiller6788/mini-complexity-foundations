/* phase_transition.c — demonstrate SAT phase transition at clause ratio 4.26 */
#include "types.h"
#include "sat.h"
#include <stdio.h>
#include <time.h>

int main(void) {
    printf("SAT Phase Transition Experiment\n");
    printf("Generating random 3-SAT formulas at varying clause/variable ratios\n\n");
    printf("ratio    n    clauses    SAT    time    decisions\n");
    printf("-------  ---  -------    ---    ----    ---------\n");
    int n = 50;
    for (double r = 2.0; r <= 7.0; r += 0.5) {
        int m = (int)(r * n);
        CNF* cnf = cnf_new(n, m);
        srand((unsigned)time(NULL));
        for (int i = 0; i < m; i++) {
            int lits[3];
            for (int j = 0; j < 3; j++) {
                int var = rand() % n;
                lits[j] = (var << 1) | (rand()&1);
            }
            cnf_add(cnf, lits, 3);
        }
        int* assign = malloc((size_t)n*sizeof(int));
        clock_t t0 = clock();
        int sat = sat_solve_dpll(cnf, assign);
        double t = (double)(clock()-t0)/CLOCKS_PER_SEC;
        printf("%-7.1f  %-5d %-9d %-5s %-7.4f ", r, n, m, sat?"SAT":"UNSAT", t);
        dpll_stats();
        free(assign); cnf_free(cnf);
    }
    printf("\nPhase transition near r=4.26: sharp SAT/UNSAT boundary.\n");
    return 0;
}
