/* diagonal.c -- Cantor-style Diagonalization for Time Hierarchy
 * Hartmanis-Stearns (1965): TIME(f) != TIME(g) when f*log f = o(g)
 * Proof: enumerate all TMs, simulate each on a diagonal input,
 * and do the opposite of what the TM would do. */

#include <stdio.h>
#include <stdlib.h>
#include "tht.h"

/* Simulate the behavior of TM #tm_id on input with step limit */
static int tm_behavior(int tm_id, int input, int step_limit) {
    int state = 0, tape_pos = 0, steps = 0;
    while (steps < step_limit) {
        int transition = (tm_id * 31337 + state * 1009 + tape_pos * 271) % 16;
        if (transition == 0) return 1;
        if (transition == 1) return 0;
        state = (state + transition) % (tm_id % 8 + 2);
        tape_pos += (transition % 3) - 1;
        steps++;
    }
    return -1;
}

/* Diagonal language: L_D = {n | TM_n does NOT accept n within g(n) steps} */
static int diagonal_language_member(int n, int (*time_bound)(int)) {
    int bound = time_bound(n);
    int result = tm_behavior(n, n, bound);
    return (result == 1) ? 0 : 1;
}

void diagonal_demo(void) {
    printf("\n===== Diagonalization: Time Hierarchy =====\n\n");
    printf("Hartmanis-Stearns (1965): TIME(f) != TIME(g)\n");
    printf("when f(n)*log f(n) = o(g(n)).\n\n");

    printf("Proof structure:\n");
    printf("  1. Enumerate all TMs: M_1, M_2, M_3, ...\n");
    printf("  2. Define L_D = {n | M_n rejects n in g(n) steps}\n");
    printf("  3. L_D in TIME(g): universal TM simulates M_n on n\n");
    printf("  4. L_D NOT in TIME(f): if M_i decides it in f(i) time,\n");
    printf("     M_i(i) = L_D(i) = NOT M_i(i) -> CONTRADICTION\n\n");

    printf("--- Concrete Example ---\n");
    printf("f(n)=n^2, g(n)=n^3. Then TIME(n^2) strict subset TIME(n^3).\n\n");

    /* Demonstrate diagonal language for small n */
    int bound_func(int x) { return 10 * x + 5; }
    printf("Diagonal language L_D (g(n)=10n+5):\n");
    for (int n = 0; n < 15; n++) {
        int tm_res = tm_behavior(n, n, 10*n + 5);
        int d = diagonal_language_member(n, bound_func);
        printf("  n=%2d: TM_%d accepts=%s, L_D=%s\n",
               n, n, tm_res==1?"YES":"NO ", d?"YES":"NO");
    }

    printf("\n--- Corollary ---\n");
    printf("P != EXP unconditionally!\n");
    printf("TIME(n^k) strict subset TIME(2^n) for any fixed k.\n");
    printf("But diagonalization relativizes -> cannot resolve P vs NP.\n");
}
