/* qbf_solver.c — Quantified Boolean Formula Solver (PSPACE-complete)
 * 
 * TQBF: Q_1 x_1 Q_2 x_2 ... Q_n x_n. phi(x_1,...,x_n)
 * where Q_i in {exists, forall}.
 *
 * Recursive evaluation: O(2^n) time, O(n) space — in PSPACE.
 * This is the canonical PSPACE-complete problem (Stockmeyer-Meyer 1973). */
#include "pspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Evaluate a single 3-CNF clause set under assignment */
static int eval_3cnf(const int* assign, int cls[][3], int nc) {
    for(int i=0;i<nc;i++) { int ok=0;
        for(int j=0;j<3;j++) { int l=cls[i][j], v=l>>1, s=l&1;
            if((!s&&assign[v])||(s&&!assign[v])) { ok=1; break; } }
        if(!ok) return 0; }
    return 1;
}

/* QBF: recursive evaluation helper.
 * assign[] carries the partial assignment through recursion.
 * var_idx: current variable being quantified (0 = outermost).
 * quants: pointer into the quantifier string at var_idx.
 * Space: O(n) for recursion depth. Time: O(2^n). */
static int qbf_eval_rec(int var_idx, int n_vars, const char* quants,
                        int cls[][3], int nc, int* assign) {
    if (var_idx >= n_vars) {
        /* All variables assigned: evaluate the propositional part */
        return eval_3cnf(assign, cls, nc);
    }
    int is_forall = (quants[0] == 'A' || quants[0] == 'a');

    /* Try assign[var_idx] = 0 */
    assign[var_idx] = 0;
    int r0 = qbf_eval_rec(var_idx + 1, n_vars, quants + 1, cls, nc, assign);

    /* Early termination */
    if (!is_forall && r0) return 1;  /* exists: one satisfying branch is enough */
    if (is_forall && !r0) return 0;  /* forall: one counterexample is enough */

    /* Try assign[var_idx] = 1 */
    assign[var_idx] = 1;
    int r1 = qbf_eval_rec(var_idx + 1, n_vars, quants + 1, cls, nc, assign);

    return is_forall ? (r0 && r1) : (r0 || r1);
}

/* QBF: recursive evaluation. quants = string of 'E'/'A' characters.
 * Space: O(n) for recursion depth. Time: O(2^n). */
int qbf_evaluate(int n_vars, const char* quants, int cls[][3], int nc) {
    int* assign = calloc((size_t)n_vars, sizeof(int));
    int result = qbf_eval_rec(0, n_vars, quants, cls, nc, assign);
    free(assign);
    return result;
}

void qbf_demo(void) {
    printf("\n===== QBF Solver (PSPACE-complete) =====\n\n");
    printf("TQBF: Q_1 x_1 ... Q_n x_n. phi(x)\n");
    printf("Q_i in {E,A} = exists, forall\n\n");
    
    printf("Example 1: exists x0 forall x1: (x0|x1) & (!x0|!x1)\n");
    int cls1[2][3] = {{(0<<1)|0, (2<<1)|0, -1}, {(0<<1)|1, (2<<1)|1, -1}};
    int r1 = qbf_evaluate(2, "EA", cls1, 2);
    printf("  Result: %s\n", r1?"TRUE":"FALSE");
    
    printf("\nExample 2: forall x0 exists x1: (x0|x1) & (!x0|!x1)\n");
    int r2 = qbf_evaluate(2, "AE", cls1, 2);
    printf("  Result: %s\n", r2?"TRUE":"FALSE");
    
    printf("\nTQBF is to PSPACE as SAT is to NP.\n");
    printf("Space complexity: O(n) recursion depth.\n");
    printf("Time complexity: O(2^n) — exponential, but space is polynomial.\n");
}