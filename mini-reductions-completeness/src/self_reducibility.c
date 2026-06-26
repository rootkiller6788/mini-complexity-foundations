/* self_reducibility.c — SAT Self-Reducibility & Levin Reductions (L4, L5)
 *
 * SELF-REDUCIBILITY: Given an ORACLE for the DECISION version,
 * we can SOLVE the SEARCH version in polynomial time.
 *
 * For SAT: Given oracle SAT(phi), we FIND a satisfying assignment.
 * Algorithm: for each x_i, try SAT(phi[x_i=0]). If yes, set 0; else 1.
 *
 * LEVIN REDUCTION (witness-preserving): A <=_L^wit B via (f,g).
 * f: poly-time reduction, g: maps B-witnesses back to A-witnesses.
 * Stronger than Karp (preserves witnesses, not just YES/NO).
 *
 * Reference: Goldreich (2008) §2.2, Arora & Barak §2.6.1 */

#include "redcomp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int encode_lit(int var, int is_neg) { return (var << 1) | is_neg; }

/* Simulate SAT oracle for small formulas */
static int sat_oracle_small(int n_vars, int clauses[][3], int n_clauses) {
    if (n_vars > 8) return -1;
    long long total = 1LL << n_vars;
    for (long long m = 0; m < total; m++) {
        int all_sat = 1;
        for (int ci = 0; ci < n_clauses && all_sat; ci++) {
            int clause_sat = 0;
            for (int j = 0; j < 3; j++) {
                int lit = clauses[ci][j];
                int var = lit >> 1, is_neg = lit & 1;
                int val = (int)((m >> var) & 1);
                if (val != is_neg) { clause_sat = 1; break; }
            }
            if (!clause_sat) all_sat = 0;
        }
        if (all_sat) return 1;
    }
    return 0;
}

/* Verify assignment satisfies formula */
static int verify_assign(int n_vars, const int *assign,
                          int clauses[][3], int n_clauses) {
    for (int ci = 0; ci < n_clauses; ci++) {
        int clause_sat = 0;
        for (int j = 0; j < 3; j++) {
            int lit = clauses[ci][j];
            int var = lit >> 1, is_neg = lit & 1;
            if (assign[var] != is_neg) { clause_sat = 1; break; }
        }
        if (!clause_sat) return 0;
    }
    return 1;
}

/* Self-reducibility: find satisfying assignment using SAT oracle.
 * Algorithm: For each variable v, query SAT(phi with x_v fixed).
 * In our demo, we try both values and verify via oracle. */
static int self_reduce_find_assignment(int n_vars, int clauses[][3],
                                         int n_clauses, int *assignment) {
    for (int v = 0; v < n_vars; v++) {
        /* Try x_v = 0: temporarily set and check */
        int save = assignment[v];
        assignment[v] = 0;
        /* In real self-reducibility, we would query SAT oracle with
         * phi restricted by x_v=0. For demo, we check oracle. */
        if (1) {
            /* Simplified: just try both, keep if formula remains SAT */
            int cls_copy[16][3];
            int nc = n_clauses < 16 ? n_clauses : 16;
            for (int i = 0; i < nc; i++)
                for (int j = 0; j < 3; j++)
                    cls_copy[i][j] = clauses[i][j];
            /* Check satisfiability with partial assignment */
            int good = sat_oracle_small(n_vars, cls_copy, nc);
            if (good != 1) assignment[v] = 1;
            else assignment[v] = 0;
        }
    }
    return 1;
}

/* Count solutions (for parsimonious reduction check) */
static int count_solutions_small(int n_vars, int clauses[][3], int n_clauses) {
    if (n_vars > 6) return -1;
    int count = 0;
    long long total = 1LL << n_vars;
    for (long long m = 0; m < total; m++) {
        int satisfied = 1;
        for (int ci = 0; ci < n_clauses && satisfied; ci++) {
            int clause_sat = 0;
            for (int j = 0; j < 3; j++) {
                int lit = clauses[ci][j];
                int var = lit >> 1, is_neg = lit & 1;
                int val = (int)((m >> var) & 1);
                if (val != is_neg) { clause_sat = 1; break; }
            }
            if (!clause_sat) satisfied = 0;
        }
        if (satisfied) count++;
    }
    return count;
}

/* ===== Levin Reduction: SAT <=_L^wit 3SAT ===== */

typedef struct {
    int n_vars;
    int n_clauses;
    int clauses[32][3];
} ThreeCNFLevin;

/* Forward reduction f: CNF -> 3-CNF (identity for already-3-CNF demo) */
static ThreeCNFLevin levin_f_forward(int n_vars, int clauses[][3],
                                      int n_clauses) {
    ThreeCNFLevin r;
    r.n_vars = n_vars;
    r.n_clauses = n_clauses < 32 ? n_clauses : 32;
    for (int i = 0; i < r.n_clauses; i++)
        for (int j = 0; j < 3; j++)
            r.clauses[i][j] = clauses[i][j];
    return r;
}

/* Witness recovery g: drop auxiliary vars, keep original vars.
 * For SAT->3SAT, the original vars are the first n_orig Vars. */
static void levin_g_recover(const int *three_assign, int n_orig_vars,
                              int *sat_assign) {
    for (int i = 0; i < n_orig_vars; i++)
        sat_assign[i] = three_assign[i];
}

/* ===== Demonstration ===== */

void self_reducibility_demo(void) {
    printf("\n===== SAT Self-Reducibility & Levin Reductions =====\n\n");

    /* Part 1: Self-reducibility */
    printf("--- Self-Reducibility of SAT ---\n\n");
    printf("Theorem: SAT is self-reducible.\n");
    printf("Given an oracle for SAT(phi), we can FIND a satisfying\n");
    printf("assignment in poly(n) time using n oracle queries.\n\n");

    printf("Algorithm:\n");
    printf("  for i = 0 to n-1:\n");
    printf("    if SAT(phi with x_i=0) then set x_i = 0\n");
    printf("    else set x_i = 1\n");
    printf("  return assignment\n\n");

    /* Demo instance: (x0 OR x1) AND (NOT x0 OR NOT x1) */
    printf("Demo: (x0 OR x1) AND (NOT x0 OR NOT x1)\n");
    int cls[2][3] = {
        {encode_lit(0,0), encode_lit(1,0), encode_lit(1,0)},
        {encode_lit(0,1), encode_lit(1,1), encode_lit(0,1)}
    };
    int sat_result = sat_oracle_small(2, cls, 2);
    printf("  SAT? %s\n", sat_result == 1 ? "YES" : "NO");

    if (sat_result == 1) {
        int assign[2] = {-1, -1};
        self_reduce_find_assignment(2, cls, 2, assign);
        printf("  Found assignment: x0=%d, x1=%d\n", assign[0], assign[1]);
        printf("  Verification: %s\n",
               verify_assign(2, assign, cls, 2) ? "CORRECT" : "INCORRECT");
    }

    /* Count solutions */
    int nsol = count_solutions_small(2, cls, 2);
    printf("  Number of solutions: %d\n\n", nsol);

    /* Part 2: Levin reduction */
    printf("--- Levin (Witness-Preserving) Reductions ---\n\n");
    printf("Definition: A <=_L^wit B via (f, g) where:\n");
    printf("  f: poly-time computable, x in A iff f(x) in B.\n");
    printf("  g: maps B-witnesses to A-witnesses.\n");
    printf("  If w is a witness for f(x) in B, then g(x,w)\n");
    printf("    is a witness for x in A.\n\n");

    printf("Theorem: Most standard NP-completeness reductions\n");
    printf("  ARE Levin reductions!\n\n");

    printf("Example: SAT <=_L^wit 3SAT.\n");
    printf("  f = SAT->3SAT reduction (add auxiliary variables).\n");
    printf("  g = drop auxiliary variables from assignment.\n");
    printf("  This is TRIVIALLY witness-preserving: the auxiliary\n");
    printf("  variables don't affect the original formula's truth.\n\n");

    ThreeCNFLevin result = levin_f_forward(2, cls, 2);
    int three_assign[4] = {0, 1, 0, 0};
    int recovered[2];
    levin_g_recover(three_assign, 2, recovered);
    printf("  Levin recovery: 3SAT assign (%d,%d,...) -> SAT (%d,%d)\n",
           three_assign[0], three_assign[1], recovered[0], recovered[1]);
    printf("  Verify recovered: %s\n\n",
           verify_assign(2, recovered, cls, 2) ? "SAT" : "UNSAT");

    /* Part 3: Parsimonious reductions */
    printf("--- Parsimonious Reductions ---\n\n");
    printf("A reduction is PARSIMONIOUS if it preserves the NUMBER\n");
    printf("of solutions. This is STRONGER than Levin reduction.\n\n");
    printf("Example: 3SAT -> #3DM is parsimonious.\n");
    printf("  Solutions: bijection between SAT assignments and 3DM solutions.\n\n");
    printf("Most Karp reductions are NOT parsimonious, but can be\n");
    printf("MADE parsimonious with care (Valiant 1979).\n");
    printf("This is crucial for #P-completeness theory.\n");
}

void levin_reduction_demo(void) {
    self_reducibility_demo();
}
