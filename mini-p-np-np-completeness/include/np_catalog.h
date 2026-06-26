#ifndef NP_CATALOG_H
#define NP_CATALOG_H
#include "types.h"

/* Additional NP reductions */
typedef struct { int n; int** adj; } Graph;
Graph* reduce_3sat_to_3color(int n_vars, int n_clauses, int clauses[][3]);
Graph* reduce_3sat_to_hamiltonian(int n_vars, int n_clauses, int clauses[][3]);

typedef struct { int* nums; int n; int target; } SubsetSum;
SubsetSum* reduce_3sat_to_subset_sum(int n_vars, int n_clauses, int clauses[][3]);
int subset_sum_solve(int* nums, int n, int target);

Graph* g_new(int n);
void g_edge(Graph* g, int u, int v);
void g_free(Graph* g);

void npc_catalog_print(void);
#endif
