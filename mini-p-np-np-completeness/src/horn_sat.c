/* horn_sat.c — Horn SAT: Polynomial-Time Special Case (P-complete)
 *
 * A Horn clause has AT MOST ONE positive literal.
 * Horn SAT is P-complete (polynomial time, but hardest in P).
 *
 * Knowledge:
 *   1. Linear-time marking algorithm (Dowling-Gallier 1984, O(n+m))
 *   2. Minimal model property: every Horn formula has a unique minimal model
 *   3. Forward chaining = logic programming (Prolog) execution
 *   4. Horn SAT = reachability in a directed hypergraph
 *   5. Contrast: 2-CNF is in P, 3-CNF is NP-complete, Horn is in P
 */

#include "horn_sat.h"
#include <string.h>
#include <stdio.h>

/* Check if each clause has ≤ 1 positive literal (definition of Horn) */
int horn_sat_is_horn(const CNF* cnf) {
    for (int i = 0; i < cnf->n_clauses; i++) {
        int pos_count = 0;
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            if (!(cnf->clauses[i].data[j] & 1)) pos_count++;
        }
        if (pos_count > 1) return 0;
    }
    return 1;
}

/* ─── Horn SAT Algorithm (O(n+m) time) ────────────────────── */
/* Key insight: all Horn clauses are implications.
   ¬x₁ ∨ ¬x₂ ∨ ... ∨ ¬x_k ∨ y  ≡  (x₁ ∧ x₂ ∧ ... ∧ x_k) → y
   Start with all variables FALSE. When antecedents of an
   implication are all TRUE, set consequent to TRUE.
   
   Minimal model property: the result is the unique minimal
   satisfying assignment (if any exists). */

int horn_sat_solve(CNF* cnf, int* assign) {
    int n = cnf->n_vars;
    for (int i = 0; i < n; i++) assign[i] = 0; /* minimal model: all false */

    /* Build implication graph: for each clause, track antecedents */
    typedef struct { int* ante; int n_ante; int consequent; int is_fact; } Impl;
    Impl* impls = malloc((size_t)cnf->n_clauses * sizeof(Impl));

    for (int i = 0; i < cnf->n_clauses; i++) {
        impls[i].ante = malloc((size_t)cnf->clauses[i].n * sizeof(int));
        impls[i].n_ante = 0;
        impls[i].consequent = -1;
        impls[i].is_fact = 0;

        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int lit = cnf->clauses[i].data[j], var = lit >> 1, sign = lit & 1;
            if (sign) impls[i].ante[impls[i].n_ante++] = var; /* neg literal = antecedent */
            else impls[i].consequent = var; /* pos literal = consequent */
        }
        if (impls[i].consequent < 0) impls[i].is_fact = 0; /* all-negative clause = constraint */
        if (impls[i].n_ante == 0 && impls[i].consequent >= 0) impls[i].is_fact = 1;
    }

    /* Build "consequent → [implications where it's an antecedent]" index */
    typedef struct { int* impl_indices; int count; } ConsequentIndex;
    ConsequentIndex* ci = calloc((size_t)n, sizeof(ConsequentIndex));

    /* Forward chaining loop */
    int* queue = malloc((size_t)n * sizeof(int));
    int qh = 0, qt = 0;

    /* Enqueue facts (clauses with no neg literals) */
    for (int i = 0; i < cnf->n_clauses; i++) {
        if (impls[i].is_fact && impls[i].consequent >= 0) {
            if (assign[impls[i].consequent] == 0) {
                assign[impls[i].consequent] = 1;
                queue[qt++] = impls[i].consequent;
            }
        }
    }

    /* Process queue */
    int* ante_remaining = malloc((size_t)cnf->n_clauses * sizeof(int));
    for (int i = 0; i < cnf->n_clauses; i++)
        ante_remaining[i] = impls[i].n_ante;

    while (qh < qt) {
        int var = queue[qh++];
        /* Check all implications where var appears as antecedent */
        for (int i = 0; i < cnf->n_clauses; i++) {
            if (impls[i].consequent < 0 || assign[impls[i].consequent] == 1) continue;
            for (int j = 0; j < impls[i].n_ante; j++) {
                if (impls[i].ante[j] == var) ante_remaining[i]--;
            }
            if (ante_remaining[i] == 0) {
                assign[impls[i].consequent] = 1;
                queue[qt++] = impls[i].consequent;
            }
        }
    }

    /* Check all-negative clauses: they must not have all antecedents true.
       If any all-neg clause has all its neg literals assigned false (=antecedents true),
       the formula is UNSAT. */
    for (int i = 0; i < cnf->n_clauses; i++) {
        if (impls[i].consequent < 0) {
            int all_ante_true = 1;
            for (int j = 0; j < impls[i].n_ante; j++)
                if (assign[impls[i].ante[j]] == 0) { all_ante_true = 0; break; }
            if (all_ante_true) {
                /* Cleanup */ for (int k=0;k<cnf->n_clauses;k++) free(impls[k].ante);
                free(impls); free(ci); free(queue); free(ante_remaining);
                return 0; /* unsatisfiable */
            }
        }
    }

    /* Cleanup */
    for (int i = 0; i < cnf->n_clauses; i++) free(impls[i].ante);
    free(impls); free(ci); free(queue); free(ante_remaining);
    return 1;
}
