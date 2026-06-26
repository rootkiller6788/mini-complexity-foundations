/* reduction.h — NP-complete reductions (Karp's 21 problems) */
#ifndef REDUCTION_H
#define REDUCTION_H
#include "types.h"

/* SAT -> 3SAT (standard reduction, preserves satisfiability) */
CNF* reduce_sat_to_3sat(const CNF* sat);

/* 3SAT -> CLIQUE */
typedef struct { int n; int** adj; } Graph;
Graph* reduce_3sat_to_clique(int n_vars, int n_clauses, int clauses[][3]);

/* 3SAT -> VERTEX COVER */
Graph* reduce_3sat_to_vc(int n_vars, int n_clauses, int clauses[][3]);

/* 3SAT -> SUBSET SUM */
typedef struct { int* numbers; int n; int target; } SubsetSum;
SubsetSum* reduce_3sat_to_subset_sum(int n_vars, int n_clauses, int clauses[][3]);

/* 3SAT -> HAMILTONIAN PATH */
Graph* reduce_3sat_to_hamiltonian(int n_vars, int n_clauses, int clauses[][3]);

/* Verify a reduction: solve original + solve reduced, compare */
int reduction_verify(CNF* original, CNF* reduced);

void graph_free(Graph* g);
void subset_sum_free(SubsetSum* ss);

#endif
