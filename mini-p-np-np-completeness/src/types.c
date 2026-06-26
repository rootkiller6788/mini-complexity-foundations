/* types.c — CNF data structure operations */
#include "types.h"
#include <stdio.h>
#include <string.h>

CNF* cnf_new(int n_vars, int n_clauses) {
    CNF* c = (CNF*)malloc(sizeof(CNF));
    c->n_vars = n_vars; c->n_clauses = n_clauses; c->add_idx = 0;
    c->clauses = (Clause*)calloc((size_t)n_clauses, sizeof(Clause));
    return c;
}

void cnf_add(CNF* c, const int* lits, int n) {
    int i = c->add_idx++;
    c->clauses[i].n = n;
    c->clauses[i].data = (int*)malloc((size_t)n * sizeof(int));
    memcpy(c->clauses[i].data, lits, (size_t)n * sizeof(int));
}

void cnf_free(CNF* c) {
    for (int i = 0; i < c->n_clauses; i++) free(c->clauses[i].data);
    free(c->clauses); free(c);
}

void cnf_print(const CNF* c) {
    printf("p cnf %d %d\n", c->n_vars, c->n_clauses);
    for (int i = 0; i < c->n_clauses; i++) {
        for (int j = 0; j < c->clauses[i].n; j++) {
            int lit = c->clauses[i].data[j];
            printf("%d ", (lit & 1) ? -(lit >> 1) - 1 : (lit >> 1) + 1);
        }
        printf("0\n");
    }
}

CNF* cnf_copy(const CNF* src) {
    CNF* dst = cnf_new(src->n_vars, src->n_clauses);
    for (int i = 0; i < src->n_clauses; i++)
        cnf_add(dst, src->clauses[i].data, src->clauses[i].n);
    return dst;
}

/* Compatibility aliases */
CNF* cnf_create(int n_vars, int n_clauses) { return cnf_new(n_vars, n_clauses); }
void cnf_add_clause(CNF* cnf, const int* lits, int n) { cnf_add(cnf, lits, n); }
