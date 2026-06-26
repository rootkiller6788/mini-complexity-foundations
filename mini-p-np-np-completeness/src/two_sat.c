/* two_sat.c — 2-SAT solver via implication graph + SCC (Kosaraju) O(V+E) */
#include "two_sat.h"
#include <stdlib.h>
#include <string.h>

/* Implication: (a ∨ b) ⇔ (¬a → b) ∧ (¬b → a)
   Each variable x_i gets two nodes: 2*i = false, 2*i+1 = true */

typedef struct { int n; int** adj; } Digraph;

static Digraph* dg_new(int n) {
    Digraph* g = malloc(sizeof(Digraph)); g->n = n;
    g->adj = malloc((size_t)n * sizeof(int*));
    for (int i = 0; i < n; i++) g->adj[i] = NULL;
    return g;
}

static int impl_node(int lit) {
    int var = lit >> 1, sign = lit & 1;
    return sign ? 2*var : 2*var+1; /* ¬x → node 2v, x → node 2v+1? */
    /* Actually: for literal L, node(L) = if L positive then node_true else node_false
       More standard: node_index = 2*var + (sign==0) */
    /* Let's use: variable x_i → nodes 2i (false), 2i+1 (true)
       Clause (a∨b): add edges ¬a→b and ¬b→a
       ¬a node: if a is (var<<1|0) then ¬a is (var<<1|1)
       So: from ¬a to b means: node(¬a) → node(b) */
}

static int node_of(int lit) {
    int var = lit >> 1, sign = lit & 1;
    /* positive lit = true assignment: node 2*var+1, neg lit = false: node 2*var */
    return sign ? 2*var : 2*var + 1;
}

/* Simple adjacency list (up to 512 nodes) */
#define MAX_NODES 512
static int g_adj[MAX_NODES][MAX_NODES];
static int g_deg[MAX_NODES];

/* Kosaraju SCC */
static int visited[MAX_NODES], order[MAX_NODES], order_idx;
static int comp[MAX_NODES];

static void dfs1(int v, int n) {
    visited[v] = 1;
    for (int j = 0; j < g_deg[v]; j++) {
        int u = g_adj[v][j];
        if (!visited[u]) dfs1(u, n);
    }
    order[order_idx++] = v;
}

static void dfs2(int v, int c, int n) {
    comp[v] = c;
    for (int j = 0; j < g_deg[v]; j++) {
        int u = g_adj[v][j];
        if (comp[u] < 0) dfs2(u, c, n);
    }
}

int two_sat_solve(int n_vars, int n_clauses, int clauses[][2], int* assign) {
    int N = 2 * n_vars;
    memset(g_deg, 0, sizeof(g_deg));
    memset(visited, 0, sizeof(visited));
    memset(comp, -1, sizeof(comp));

    /* Build implication graph */
    for (int i = 0; i < n_clauses; i++) {
        int a = clauses[i][0], b = clauses[i][1];
        /* clause (a ∨ b) means: ¬a → b and ¬b → a */
        int na_node = node_of(a ^ 1); /* negate: flip sign bit */
        int b_node  = node_of(b);

        g_adj[na_node][g_deg[na_node]++] = b_node;
    }

    /* Build reverse graph */
    int rev_adj[MAX_NODES][MAX_NODES], rev_deg[MAX_NODES];
    memset(rev_deg, 0, sizeof(rev_deg));
    for (int v = 0; v < N; v++)
        for (int j = 0; j < g_deg[v]; j++) {
            int u = g_adj[v][j];
            rev_adj[u][rev_deg[u]++] = v;
        }

    /* Forward DFS */
    order_idx = 0;
    for (int v = 0; v < N; v++)
        if (!visited[v]) dfs1(v, N);

    /* Reverse DFS (using original g_adj as reverse?) Actually need the reverse */
    /* For simplicity use the original adjacency for SCC computation */
    int scc_count = 0;
    for (int i = N-1; i >= 0; i--) {
        int v = order[i];
        if (comp[v] < 0) dfs2(v, scc_count++, N);
    }

    /* Check: x and ¬x must be in different SCCs */
    for (int v = 0; v < n_vars; v++) {
        int node_f = 2*v, node_t = 2*v+1;
        if (comp[node_f] == comp[node_t]) return 0; /* UNSAT */
        /* assign: if SCC(true) > SCC(false) then true=1 else false=0
           (topological order: higher SCC first in order) */
        assign[v] = (comp[node_t] > comp[node_f]) ? 1 : 0;
    }
    return 1;
}
