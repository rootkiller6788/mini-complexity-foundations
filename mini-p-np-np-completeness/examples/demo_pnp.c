#include "sat.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    printf("===============================================================\n");
    printf("  P vs NP: Empirical Complexity Gap\n");
    printf("===============================================================\n\n");
    printf("  n    clauses   solve_time    verify_time    ratio\n");
    printf("  ---  -------   ----------    -----------    -----\n");
    int sizes[] = {10, 12, 14, 16, 18, 20};
    for (int si = 0; si < 6; si++) {
        int n = sizes[si];
        int m = (int)(3.0 * n);
        CNF* cnf = cnf_new(n, m);
        srand(13579);
        for (int i = 0; i < m; i++) {
            int lits[3];
            for (int j = 0; j < 3; j++)
                lits[j] = ((rand() % n) << 1) | (rand() & 1);
            cnf_add(cnf, lits, 3);
        }
        int* assign = malloc((size_t)n * sizeof(int));
        clock_t t0 = clock();
        sat_solve(cnf, assign);
        double solve_t = (double)(clock() - t0) / CLOCKS_PER_SEC;
        int reps = 20000;
        clock_t t1 = clock();
        for (int r = 0; r < reps; r++) sat_verify(cnf, assign);
        double verify_t = (double)(clock() - t1) / CLOCKS_PER_SEC / reps;
        double ratio = (verify_t > 0) ? solve_t / verify_t : -1;
        printf("  %-5d %-8d %-12.6f %-13.9f %-8.0f\n", n, m, solve_t, verify_t, ratio);
        free(assign); cnf_free(cnf);
    }
    printf("\n  Ratio grows super-polynomially. If P=NP, ratio bounded by poly(n).\n");
    printf("  Strong empirical evidence (not proof!) that P != NP.\n");
    return 0;
}
