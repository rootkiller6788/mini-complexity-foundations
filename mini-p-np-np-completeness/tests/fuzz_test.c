/* fuzz_test.c — random formula fuzz test: solve == verify */
#include "types.h"
#include "sat.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    printf("===== Fuzz Test: random formulas =====\n");
    srand((unsigned)time(NULL));
    int total = 0, passed = 0;
    for (int round = 0; round < 20; round++) {
        int n = 8 + rand() % 15;
        int m = 5 + rand() % (int)(5.0 * n);
        CNF* cnf = cnf_new(n, m);
        for (int i = 0; i < m; i++) {
            int len = 2 + rand() % 3;
            int lits[3];
            for (int j = 0; j < len; j++)
                lits[j] = ((rand() % n) << 1) | (rand() & 1);
            cnf_add(cnf, lits, len);
        }
        int* a = malloc((size_t)n * sizeof(int));
        int r = sat_solve(cnf, a);
        int ok = 1;
        if (r) { if (!sat_verify(cnf, a)) ok = 0; }
        else { /* verify no assignment works (sample check) */
            for (int s = 0; s < 1000 && ok; s++) {
                for (int v = 0; v < n; v++) a[v] = rand() & 1;
                if (sat_verify(cnf, a)) ok = 0;
            }
        }
        total++;
        if (ok) { passed++; printf("  round %2d: n=%2d m=%2d %s PASS\n", round, n, m, r?"SAT":"UNSAT"); }
        else     { printf("  round %2d: n=%2d m=%2d %s FAIL!\n", round, n, m, r?"SAT":"UNSAT"); }
        free(a); cnf_free(cnf);
    }
    printf("\n===== %d/%d fuzz tests passed =====\n", passed, total);
    return (passed == total) ? 0 : 1;
}
