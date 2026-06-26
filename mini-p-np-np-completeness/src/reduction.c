/* reduction.c — NP-complete reductions (Karp's catalog) */
#include "reduction.h"
#include "sat.h"
#include <stdio.h>
#include <string.h>

/* SAT -> 3SAT: split long clauses with auxiliary variables */
CNF* reduce_sat_to_3sat(const CNF* sat) {
    int new_clauses = 0;
    for (int i = 0; i < sat->n_clauses; i++) {
        int n = sat->clauses[i].n;
        if (n <= 3) new_clauses += 1;
        else new_clauses += n - 2; /* split into (n-2) 3-clauses */
    }
    int extra_vars = 0;
    for (int i = 0; i < sat->n_clauses; i++)
        if (sat->clauses[i].n > 3) extra_vars += sat->clauses[i].n - 3;

    CNF* cnf = cnf_new(sat->n_vars + extra_vars, new_clauses);

    int aux_var = sat->n_vars;
    for (int i = 0; i < sat->n_clauses; i++) {
        int n = sat->clauses[i].n;
        if (n <= 3) {
            cnf_add(cnf, sat->clauses[i].data, n);
        } else {
            /* (l1|l2|aux1) & (!aux1|l3|aux2) & (!aux2|l4|aux3) ... & (!aux_{n-3}|l_{n-1}|l_n) */
            for (int k = 0; k < n - 2; k++) {
                int cl[3];
                if (k == 0) {
                    cl[0] = sat->clauses[i].data[0];
                    cl[1] = sat->clauses[i].data[1];
                    cl[2] = (aux_var << 1) | 0; /* positive auxiliary */
                } else if (k == n - 3) {
                    cl[0] = ((aux_var + k - 1) << 1) | 1; /* negate previous aux */
                    cl[1] = sat->clauses[i].data[n - 2];
                    cl[2] = sat->clauses[i].data[n - 1];
                } else {
                    cl[0] = ((aux_var + k - 1) << 1) | 1;
                    cl[1] = sat->clauses[i].data[k + 1];
                    cl[2] = (aux_var + k) << 1;
                }
                cnf_add(cnf, cl, 3);
            }
            aux_var += n - 3;
        }
    }
    return cnf;
}

/* 3SAT -> CLIQUE reduction */
Graph* reduce_3sat_to_clique(int n_vars, int n_clauses, int clauses[][3]) {
    int V = 3 * n_clauses;
    Graph* g = (Graph*)malloc(sizeof(Graph));
    g->n = V;
    g->adj = (int**)malloc((size_t)V * sizeof(int*));
    for (int i = 0; i < V; i++) {
        g->adj[i] = (int*)calloc((size_t)V, sizeof(int));
    }
    for (int i = 0; i < n_clauses; i++) {
        for (int a = 0; a < 3; a++) {
            for (int j = 0; j < n_clauses; j++) {
                if (i == j) continue;
                for (int b = 0; b < 3; b++) {
                    int lit_i = clauses[i][a], lit_j = clauses[j][b];
                    int var_i = lit_i >> 1, var_j = lit_j >> 1;
                    int sign_i = lit_i & 1, sign_j = lit_j & 1;
                    if (var_i != var_j || sign_i == sign_j)
                        g->adj[3*i + a][3*j + b] = 1;
                }
            }
        }
    }
    return g;
}

/* 3SAT -> VERTEX COVER */
Graph* reduce_3sat_to_vc(int n_vars, int n_clauses, int clauses[][3]) {
    /* For each variable x: edge (x, !x) — need to cover one
       For each clause (a|b|c): triangle on (a,b,c) — need to cover 2
       k = n_vars + 2*n_clauses */
    int n_literals = 2 * n_vars;
    int V = n_literals + 3 * n_clauses;
    Graph* g = (Graph*)malloc(sizeof(Graph));
    g->n = V;
    g->adj = (int**)malloc((size_t)V * sizeof(int*));
    for (int i = 0; i < V; i++)
        g->adj[i] = (int*)calloc((size_t)V, sizeof(int));

    /* variable edges */
    for (int v = 0; v < n_vars; v++) {
        g->adj[2*v][2*v+1] = g->adj[2*v+1][2*v] = 1;
    }
    /* clause triangles */
    for (int i = 0; i < n_clauses; i++) {
        int base = n_literals + 3*i;
        g->adj[base][base+1] = g->adj[base+1][base] = 1;
        g->adj[base+1][base+2] = g->adj[base+2][base+1] = 1;
        g->adj[base+2][base] = g->adj[base][base+2] = 1;
    }
    return g;
}

int reduction_verify(CNF* original, CNF* reduced) {
    int* a1 = (int*)malloc((size_t)original->n_vars * sizeof(int));
    int* a2 = (int*)malloc((size_t)reduced->n_vars * sizeof(int));
    int sat1 = sat_solve(original, a1);
    int sat2 = sat_solve(reduced, a2);
    free(a1); free(a2);
    return (sat1 == sat2);
}

void graph_free(Graph* g) {
    for (int i = 0; i < g->n; i++) free(g->adj[i]);
    free(g->adj); free(g);
}

void subset_sum_free(SubsetSum* ss) { free(ss->numbers); free(ss); }
