/*
 * dpll.c — DPLL SAT Solver (Davis-Putnam-Logemann-Loveland)
 *
 * L5 Algorithm:
 *   DPLL is a complete, backtracking-based search algorithm for SAT.
 *   It combines:
 *     - Unit propagation: if a clause has all literals false except one
 *       unassigned, that literal must be true.
 *     - Pure literal elimination: if a variable appears only positively (or
 *       only negatively), assign it the satisfying value.
 *     - Branching: choose an unassigned variable, try both values.
 *
 * Algorithm DPLL(phi, assign):
 *   1. Unit propagate until fixpoint
 *   2. Pure literal eliminate until fixpoint
 *   3. If all clauses satisfied: return SAT
 *   4. If any clause is empty (conflict): return UNSAT
 *   5. Choose unassigned variable v
 *   6. If DPLL(phi, assign[v:=0]) = SAT: return SAT
 *   7. Else return DPLL(phi, assign[v:=1])
 *
 * Complexity: O(2^n) worst-case, often much faster in practice.
 *
 * Historical Note:
 *   Davis-Putnam (1960) introduced the resolution-based procedure.
 *   Davis-Logemann-Loveland (1962) replaced resolution with search,
 *   creating the DPLL algorithm used today.
 *
 * Reference: Davis, Logemann, Loveland (CACM 1962); Arora-Barak section 2.4
 * Courses: MIT 6.045, Stanford CS254, Berkeley CS172, CMU 15-455
 */

#include "sat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Solver statistics */
static long long dpll_node_count = 0;
static long long dpll_propagation_count = 0;
static long long dpll_pure_literal_count = 0;
static long long dpll_backtrack_count = 0;
static int      dpll_max_depth = 0;

/* ================================================================
 * Unit Propagation
 * ================================================================ */

/*
 * Find unit clauses: clauses where all literals are false except
 * one that is unassigned. Force that literal to true.
 *
 * Returns number of variables assigned during this pass.
 *
 * Complexity: O(n_clauses * max_clause_size) per pass.
 */
int dpll_unit_propagate(CNF* cnf, Assignment assign) {
    int changes = 0;
    for (int i = 0; i < cnf->n_clauses; i++) {
        const Clause* c = &cnf->clauses[i];
        int undef_var = -1, undef_sign = -1, undef_count = 0;
        int satisfied = 0;

        for (int j = 0; j < c->n; j++) {
            int lit = c->lits[j];
            int var = SAT_LIT_VAR(lit);
            int sign = SAT_LIT_SIGN(lit);
            int val = assign[var];

            if (val < 0) {
                undef_var = var;
                undef_sign = sign;
                undef_count++;
            } else if ((!sign && val == 1) || (sign && val == 0)) {
                satisfied = 1;
                break;
            }
            /* else: literal is false, continue */
        }

        if (!satisfied && undef_count == 1) {
            /* Unit clause: force the single unassigned literal */
            assign[undef_var] = undef_sign ? 0 : 1;
            changes++;
            dpll_propagation_count++;
        }
    }
    return changes;
}

/* ================================================================
 * Pure Literal Elimination
 * ================================================================ */

/*
 * A literal is pure if it appears in the formula but its negation
 * does not. Pure literals can be assigned true without affecting
 * satisfiability.
 *
 * Returns 1 if any pure literal was assigned.
 *
 * Complexity: O(total_literals)
 */
int dpll_pure_literal(CNF* cnf, Assignment assign) {
    int n = cnf->n_vars;
    int* pos_occurs = (int*)calloc((size_t)n, sizeof(int));
    int* neg_occurs = (int*)calloc((size_t)n, sizeof(int));

    /* Count occurrences of each literal sign */
    for (int i = 0; i < cnf->n_clauses; i++) {
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int lit = cnf->clauses[i].lits[j];
            int var = SAT_LIT_VAR(lit);
            if (SAT_LIT_SIGN(lit))
                neg_occurs[var] = 1;
            else
                pos_occurs[var] = 1;
        }
    }

    int assigned = 0;
    for (int v = 0; v < n; v++) {
        if (assign[v] < 0) {
            if (pos_occurs[v] && !neg_occurs[v]) {
                assign[v] = 1;
                assigned = 1;
                dpll_pure_literal_count++;
            } else if (!pos_occurs[v] && neg_occurs[v]) {
                assign[v] = 0;
                assigned = 1;
                dpll_pure_literal_count++;
            }
        }
    }

    free(pos_occurs);
    free(neg_occurs);
    return assigned;
}

/* ================================================================
 * Variable Selection Heuristic
 * ================================================================ */

/*
 * Choose the most frequently occurring unassigned variable.
 * This is a simple heuristic; modern solvers use VSIDS or similar.
 *
 * Returns the variable index, or -1 if all variables are assigned.
 */
int dpll_choose_branch(const CNF* cnf, const Assignment assign) {
    int n = cnf->n_vars;
    int* freq = (int*)calloc((size_t)n, sizeof(int));

    for (int i = 0; i < cnf->n_clauses; i++) {
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int var = SAT_LIT_VAR(cnf->clauses[i].lits[j]);
            if (assign[var] < 0)
                freq[var]++;
        }
    }

    int best_var = -1, best_freq = -1;
    for (int v = 0; v < n; v++) {
        if (assign[v] < 0 && freq[v] > best_freq) {
            best_var = v;
            best_freq = freq[v];
        }
    }

    free(freq);
    return best_var;
}

/* ================================================================
 * Recursive DPLL Core
 * ================================================================ */

static int dpll_rec(CNF* cnf, Assignment assign, int depth) {
    dpll_node_count++;
    if (depth > dpll_max_depth) dpll_max_depth = depth;

    /* Unit propagation loop — run until fixpoint */
    int propagated;
    do {
        propagated = dpll_unit_propagate(cnf, assign);
    } while (propagated > 0);

    /* Pure literal elimination loop */
    int purified;
    do {
        purified = dpll_pure_literal(cnf, assign);
    } while (purified);

    /* Check formula status under current partial assignment */
    int all_sat = 1;
    for (int i = 0; i < cnf->n_clauses; i++) {
        int cv = clause_value(&cnf->clauses[i], assign);
        if (cv == 0) {
            /* Conflict: this branch is unsatisfiable */
            dpll_backtrack_count++;
            return 0;
        }
        if (cv == -1) all_sat = 0;  /* undetermined */
    }

    if (all_sat) return 1;  /* all clauses satisfied */

    /* Choose branching variable */
    int pivot = dpll_choose_branch(cnf, assign);
    if (pivot < 0) return 1;  /* all variables assigned, formula SAT */

    /* Save current assignment for backtracking */
    int n = cnf->n_vars;
    Assignment saved = (Assignment)malloc((size_t)n * sizeof(int));
    memcpy(saved, assign, (size_t)n * sizeof(int));

    /* Try FALSE first (common heuristic: false branch often fails fast) */
    assign[pivot] = 0;
    if (dpll_rec(cnf, assign, depth + 1)) {
        free(saved);
        return 1;
    }

    /* Backtrack and try TRUE */
    memcpy(assign, saved, (size_t)n * sizeof(int));
    assign[pivot] = 1;
    if (dpll_rec(cnf, assign, depth + 1)) {
        free(saved);
        return 1;
    }

    /* Both branches failed — UNSAT */
    memcpy(assign, saved, (size_t)n * sizeof(int));
    free(saved);
    return 0;
}

/* ================================================================
 * Public DPLL Interface
 * ================================================================ */

int sat_solve_dpll(CNF* cnf, Assignment assign) {
    if (!cnf || !assign) return 0;

    /* Initialize all variables to unassigned */
    for (int i = 0; i < cnf->n_vars; i++)
        assign[i] = -1;

    dpll_node_count = 0;
    dpll_propagation_count = 0;
    dpll_pure_literal_count = 0;
    dpll_backtrack_count = 0;
    dpll_max_depth = 0;

    int result = dpll_rec(cnf, assign, 0);
    return result;
}

/* ================================================================
 * Solver Statistics
 * ================================================================ */

void sat_dpll_stats(void) {
    printf("DPLL Statistics:\n");
    printf("  Nodes explored:     %lld\n", dpll_node_count);
    printf("  Unit propagations:  %lld\n", dpll_propagation_count);
    printf("  Pure literal elims: %lld\n", dpll_pure_literal_count);
    printf("  Backtracks:         %lld\n", dpll_backtrack_count);
    printf("  Max recursion depth: %d\n", dpll_max_depth);
    printf("  Effective branching factor: %.2f\n",
           dpll_node_count > 0 ?
           (double)dpll_backtrack_count / (double)dpll_node_count : 0.0);
}