/* types.h — core types for SAT/NP module */
#ifndef TYPES_H
#define TYPES_H
#include <stdlib.h>

typedef struct { int* data; int n; } Clause;

typedef struct {
    Clause* clauses;
    int     n_clauses;
    int     n_vars;
    int     add_idx;
} CNF;

CNF* cnf_new(int n_vars, int n_clauses);
CNF* cnf_create(int n_vars, int n_clauses);       /* alias for backward compatibility */
void cnf_add(CNF* cnf, const int* lits, int n);
void cnf_add_clause(CNF* cnf, const int* lits, int n); /* alias for backward compatibility */
void cnf_free(CNF* cnf);
void cnf_print(const CNF* cnf);
CNF* cnf_copy(const CNF* cnf);

#endif
