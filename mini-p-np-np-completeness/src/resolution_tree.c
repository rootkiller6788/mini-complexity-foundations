/* resolution_tree.c — DAG-based resolution proof visualization */
#include "resolution.h"
#include <stdio.h>
#include <string.h>

/* Build a resolution proof DAG for unsatisfiable formulas.
   Given a set of clauses, exhaustively apply resolution
   until empty clause derived or resource exhausted. */

#define MAX_CLAUSES 256
#define MAX_LITS    32

typedef struct {
    int  lits[MAX_LITS];
    int  n;
    int  parent_a, parent_b; /* clause indices, -1 if original */
    int  pivot_var;          /* variable resolved on */
    int  is_original;
} ResClause;

static ResClause db[MAX_CLAUSES];
static int db_count;

/* Initialize proof database from CNF */
static void init_db(const CNF* cnf) {
    db_count = cnf->n_clauses;
    for (int i = 0; i < cnf->n_clauses; i++) {
        for (int j = 0; j < cnf->clauses[i].n && j < MAX_LITS; j++)
            db[i].lits[j] = cnf->clauses[i].data[j];
        db[i].n = cnf->clauses[i].n;
        db[i].parent_a = db[i].parent_b = -1;
        db[i].pivot_var = -1;
        db[i].is_original = 1;
    }
}

/* Try to resolve clause a with clause b.
   Returns 1 if successful, stores result in db[db_count]. */
static int try_resolve(int a, int b) {
    ResClause* ca = &db[a], *cb = &db[b];
    int pivot_var = -1, pivot_count = 0;

    /* Find complementary literals */
    for (int i = 0; i < ca->n; i++) {
        int ai = ca->lits[i], av = ai >> 1, as = ai & 1;
        for (int j = 0; j < cb->n; j++) {
            int bj = cb->lits[j], bv = bj >> 1, bs = bj & 1;
            if (av == bv && as != bs) {
                if (pivot_count == 0) { pivot_var = av; }
                if (av == pivot_var) pivot_count++;
            }
        }
    }
    if (pivot_count != 1) return 0; /* need exactly one complementary pair */

    /* Build resolvent */
    ResClause* res = &db[db_count];
    res->n = 0;

    for (int i = 0; i < ca->n; i++) {
        int v = ca->lits[i] >> 1;
        if (v == pivot_var) continue;
        /* check duplicate */
        int dup = 0;
        for (int k = 0; k < res->n; k++)
            if (res->lits[k] == ca->lits[i]) { dup = 1; break; }
        if (!dup && res->n < MAX_LITS) res->lits[res->n++] = ca->lits[i];
    }
    for (int j = 0; j < cb->n; j++) {
        int v = cb->lits[j] >> 1;
        if (v == pivot_var) continue;
        int dup = 0;
        for (int k = 0; k < res->n; k++)
            if (res->lits[k] == cb->lits[j]) { dup = 1; break; }
        if (!dup && res->n < MAX_LITS) res->lits[res->n++] = cb->lits[j];
    }

    res->parent_a = a; res->parent_b = b;
    res->pivot_var = pivot_var;
    res->is_original = 0;
    db_count++;
    return 1;
}

/* Run resolution proof search */
int resolution_refute_full(CNF* cnf, int max_steps, int* proof_steps) {
    init_db(cnf);
    *proof_steps = 0;

    for (int step = 0; step < max_steps; step++) {
        int progress = 0;
        for (int i = 0; i < db_count && db_count < MAX_CLAUSES; i++) {
            for (int j = 0; j < db_count && db_count < MAX_CLAUSES; j++) {
                if (i == j) continue;
                if (try_resolve(i, j)) {
                    (*proof_steps)++;
                    progress = 1;
                    /* Found empty clause? */
                    if (db[db_count-1].n == 0) return 1;
                }
            }
        }
        if (!progress) return 0; /* saturated */
    }
    return 0; /* resource exhausted */
}

/* Print the resolution proof DAG */
void print_proof_dag(int target) {
    if (target < 0 || target >= db_count) return;

    ResClause* c = &db[target];
    if (c->is_original) {
        printf("  [C%d] ORIGINAL: (", target);
        for (int i = 0; i < c->n; i++) {
            int lit = c->lits[i], var = lit >> 1, sign = lit & 1;
            if (i > 0) printf("|");
            printf("%sx%d", sign ? "!" : "", var);
        }
        printf(")\n");
        return;
    }

    printf("  [C%d] RESOLVE(pivot=x%d):\n", target, c->pivot_var);
    print_proof_dag(c->parent_a);
    print_proof_dag(c->parent_b);
    if (c->n == 0)
        printf("    => EMPTY CLAUSE (contradiction!)\n");
}
