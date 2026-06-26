/* test_sat.c — SAT solver correctness tests */
#include "types.h"
#include "sat.h"
#include "dpll.h"
#include "circuit.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int passed = 0, total = 0;

static void check(int cond, const char* name) {
    total++;
    if (cond) { passed++; printf("  PASS: %s\n", name); }
    else      { printf("  FAIL: %s\n", name); }
    fflush(stdout);
}

int main(void) {
    setbuf(stdout, NULL);
    printf("\n===== SAT Solver Tests =====\n\n");

    /* Test 1: simple SAT */
    CNF* f = cnf_new(2, 2);
    int c1[] = { (0<<1)|0, (1<<1)|0 };
    int c2[] = { (0<<1)|1, (1<<1)|1 };
    cnf_add(f, c1, 2);
    cnf_add(f, c2, 2);
    int a[2] = {-1, -1};
    int r = sat_solve(f, a);
    check(r == 1, "Simple SAT (x0|x1)&(!x0|!x1)");
    if (r) check(sat_verify(f, a) == 1, "Verifier confirms");
    cnf_free(f);

    /* Test 2: UNSAT */
    CNF* g = cnf_new(1, 2);
    int d1[] = { (0<<1)|0 };
    int d2[] = { (0<<1)|1 };
    cnf_add(g, d1, 1);
    cnf_add(g, d2, 1);
    int b[1] = {-1};
    check(sat_solve(g, b) == 0, "UNSAT (x0)&(!x0)");
    cnf_free(g);

    /* Test 3: 3-SAT */
    CNF* h = cnf_new(3, 3);
    int e1[] = { (0<<1)|0, (1<<1)|0, (2<<1)|0 };
    int e2[] = { (0<<1)|1, (1<<1)|1, (2<<1)|0 };
    int e3[] = { (0<<1)|0, (1<<1)|1, (2<<1)|1 };
    cnf_add(h, e1, 3);
    cnf_add(h, e2, 3);
    cnf_add(h, e3, 3);
    int c[3] = {-1, -1, -1};
    r = sat_solve(h, c);
    check(r == 1, "3-SAT satisfiable");
    if (r) check(sat_verify(h, c) == 1, "Verifier confirms 3-SAT");
    cnf_free(h);

    /* Test 4: #SAT counting */
    CNF* k = cnf_new(2, 1);
    int f1[] = { (0<<1)|0, (1<<1)|0 };
    cnf_add(k, f1, 2);
    long long cnt = sat_count_exact(k);
    check(cnt == 3, "#SAT: (x0|x1) has 3 solutions");
    cnf_free(k);

    /* Test 5: circuit evaluation */
    printf("\n-- Circuit tests --\n");
    /* Build circuit: (x0 AND x1) OR (NOT x2) */
    Circuit* cir = circuit_new(3, 20);
    circuit_add_input(cir, 0);
    circuit_add_input(cir, 1);
    circuit_add_input(cir, 2);
    int a0 = circuit_add_and(cir, 0, 1);
    int n2 = circuit_add_not(cir, 2);
    int root = circuit_add_or(cir, a0, n2);
    circuit_set_output(cir, root);
    int inp[] = {1,1,0};
    int ev = circuit_eval(cir, inp);
    check(ev == 1, "Circuit eval: (1&1)|(!0)=1");
    int inp2[] = {0,0,1};
    check(circuit_eval(cir, inp2) == 0, "Circuit eval: (0&0)|(!1)=0");
    check(circuit_size(cir) == 6, "Circuit: 6 gates");
    circuit_free(cir);

    /* Test 6: random at phase transition */
    printf("\nPerformance (random 3-SAT, ratio=4.26):\n");
    for (int n_idx = 0; n_idx < 3; n_idx++) {
        int nvals[] = {15, 22, 30};
        int n = nvals[n_idx];
        int m = (int)(4.26 * n);
        CNF* rnd = cnf_new(n, m);
        srand(42 + n_idx);
        for (int i = 0; i < m; i++) {
            int lits[3];
            for (int j = 0; j < 3; j++)
                lits[j] = ((rand() % n) << 1) | (rand() & 1);
            cnf_add(rnd, lits, 3);
        }
        int* assign = malloc((size_t)n * sizeof(int));
        clock_t t0 = clock();
        int sat = sat_solve(rnd, assign);
        double t = (double)(clock() - t0) / CLOCKS_PER_SEC;
        printf("  n=%-3d m=%-3d  %s  %.3fs\n", n, m, sat ? "SAT " : "UNSAT", t);
        if (sat) check(sat_verify(rnd, assign) == 1, "random verify");
        cnf_free(rnd); free(assign);
    }

    printf("\n===== %d/%d PASSED =====\n", passed, total);
    return (passed == total) ? 0 : 1;
}
