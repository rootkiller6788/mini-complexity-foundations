/* dpll.c — DPLL SAT solver (Davis-Putnam-Logemann-Loveland) */
#include "dpll.h"
#include <string.h>
#include <stdio.h>

static long long dpll_nodes = 0;
static long long dpll_propagations = 0;

static int clause_value(const Clause* c, const int* assign) {
    int any_undef = 0;
    for (int j = 0; j < c->n; j++) {
        int lit = c->data[j], var = lit >> 1, sign = lit & 1;
        int val = assign[var];
        if (val < 0) { any_undef = 1; continue; }
        if ((!sign && val) || (sign && !val)) return 1;
    }
    return any_undef ? -1 : 0;
}

int dpll_unit_propagate(CNF* cnf, int* assign) {
    int changes = 0;
    for (int i = 0; i < cnf->n_clauses; i++) {
        const Clause* c = &cnf->clauses[i];
        int undef_var = -1, undef_sign = -1, undef_count = 0;
        int sat = 0;
        for (int j = 0; j < c->n; j++) {
            int lit = c->data[j], var = lit >> 1, sign = lit & 1;
            int val = assign[var];
            if (val < 0) { undef_var = var; undef_sign = sign; undef_count++; }
            else if ((!sign && val) || (sign && !val)) { sat = 1; break; }
        }
        if (!sat && undef_count == 1) {
            assign[undef_var] = undef_sign ? 0 : 1;
            changes++; dpll_propagations++;
        }
    }
    return changes;
}

int dpll_pure_literal(CNF* cnf, int* assign) {
    int n = cnf->n_vars;
    int* pos = (int*)calloc((size_t)n, sizeof(int));
    int* neg = (int*)calloc((size_t)n, sizeof(int));
    for (int i = 0; i < cnf->n_clauses; i++)
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int lit = cnf->clauses[i].data[j];
            int var = lit >> 1;
            if (lit & 1) neg[var] = 1; else pos[var] = 1;
        }
    for (int v = 0; v < n; v++) {
        if (assign[v] < 0) {
            if (pos[v] && !neg[v]) { assign[v] = 1; free(pos); free(neg); return 1; }
            if (!pos[v] && neg[v]) { assign[v] = 0; free(pos); free(neg); return 1; }
        }
    }
    free(pos); free(neg);
    return 0;
}

int dpll_choose_branch(const CNF* cnf, const int* assign, int n_vars) {
    /* Most frequent unassigned variable heuristic */
    int* freq = (int*)calloc((size_t)n_vars, sizeof(int));
    for (int i = 0; i < cnf->n_clauses; i++)
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int var = cnf->clauses[i].data[j] >> 1;
            if (assign[var] < 0) freq[var]++;
        }
    int best = -1, best_freq = -1;
    for (int v = 0; v < n_vars; v++)
        if (assign[v] < 0 && freq[v] > best_freq) { best = v; best_freq = freq[v]; }
    free(freq);
    return best;
}

static int dpll_rec(CNF* cnf, int* assign, int depth) {
    dpll_nodes++;
    int n = cnf->n_vars;

    /* unit propagation */
    while (dpll_unit_propagate(cnf, assign) > 0);

    /* pure literal */
    while (dpll_pure_literal(cnf, assign));

    /* check status */
    int all_true = 1;
    for (int i = 0; i < cnf->n_clauses; i++) {
        int s = clause_value(&cnf->clauses[i], assign);
        if (s == 0) return 0;     /* conflict */
        if (s == -1) all_true = 0;
    }
    if (all_true) return 1;

    /* branch */
    int pivot = dpll_choose_branch(cnf, assign, n);
    if (pivot < 0) return 1;

    int* saved = (int*)malloc((size_t)n * sizeof(int));
    memcpy(saved, assign, (size_t)n * sizeof(int));

    assign[pivot] = 0;
    if (dpll_rec(cnf, assign, depth + 1)) { free(saved); return 1; }

    memcpy(assign, saved, (size_t)n * sizeof(int));
    assign[pivot] = 1;
    if (dpll_rec(cnf, assign, depth + 1)) { free(saved); return 1; }

    memcpy(assign, saved, (size_t)n * sizeof(int));
    free(saved);
    return 0;
}

int dpll_solve(CNF* cnf, int* assign) {
    for (int i = 0; i < cnf->n_vars; i++) assign[i] = -1;
    dpll_nodes = 0; dpll_propagations = 0;
    return dpll_rec(cnf, assign, 0);
}

void dpll_stats(void) {
    printf("DPLL: %lld nodes, %lld propagations\n", dpll_nodes, dpll_propagations);
}
