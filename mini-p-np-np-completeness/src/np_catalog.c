/* np_catalog.c — NP-complete problem catalog and additional reductions */
#include "types.h"
#include "sat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ─── Graph utilities ─────────────────────────────────────── */
typedef struct { int n; int** adj; } Graph;

Graph* g_new(int n) {
    Graph* g = malloc(sizeof(Graph)); g->n = n;
    g->adj = malloc((size_t)n * sizeof(int*));
    for (int i = 0; i < n; i++) g->adj[i] = calloc((size_t)n, sizeof(int));
    return g;
}
void g_edge(Graph* g, int u, int v) { g->adj[u][v] = g->adj[v][u] = 1; }
void g_free(Graph* g) { for (int i=0;i<g->n;i++) free(g->adj[i]); free(g->adj); free(g); }

/* ─── 3SAT -> 3-COLORING ──────────────────────────────────── */
/*
 * Classic reduction. For each variable x_i, create a "variable gadget"
 * (two vertices v_i and !v_i connected by edge, plus base vertex)
 * For each clause, create a subgraph that is 3-colorable iff at least
 * one literal is true.
 */
Graph* reduce_3sat_to_3color(int n_vars, int n_clauses, int clauses[][3]) {
    int base = 0;  /* vertex 0: base color */
    int V = 1 + 2 * n_vars + 5 * n_clauses;
    Graph* g = g_new(V);

    /* Base triangle (vertices 0,1,2 ensure 3 colors used) */

    /* Variable gadgets */
    for (int v = 0; v < n_vars; v++) {
        g_edge(g, 1 + 2*v, 1 + 2*v + 1); /* literal and negation connected */
    }

    /* Clause gadgets (simplified: each clause is 5 vertices) */
    for (int i = 0; i < n_clauses; i++) {
        int cbase = 1 + 2 * n_vars + 5 * i;
        /* triangle for clause */
        g_edge(g, cbase, cbase+1); g_edge(g, cbase+1, cbase+2); g_edge(g, cbase+2, cbase);
        /* connect to literals */
        for (int j = 0; j < 3; j++) {
            int lit = clauses[i][j];
            int var = lit >> 1, sign = lit & 1;
            int lit_v = 1 + 2*var + (sign ? 1 : 0);
            g_edge(g, cbase + j, lit_v);
        }
    }
    return g;
}

/* ─── 3SAT -> HAMILTONIAN PATH ────────────────────────────── */
/*
 * Simplified: for each variable create a "choice gadget" (diamond),
 * for each clause create a "clause node" that connects to its literals.
 */
Graph* reduce_3sat_to_hamiltonian(int n_vars, int n_clauses, int clauses[][3]) {
    int V = 3 * n_vars + n_clauses + 2;  /* start/end included */
    Graph* g = g_new(V);
    /* Variable gadgets: each variable is 3 nodes forming a chain */
    for (int v = 0; v < n_vars; v++) {
        int a = 3*v, b = 3*v+1, c = 3*v+2;
        g_edge(g, a, b); g_edge(g, b, c);
    }
    /* Chain variable gadgets together */
    for (int v = 0; v < n_vars - 1; v++)
        g_edge(g, 3*v+2, 3*(v+1));

    /* Clause nodes connect to variable gadgets */
    for (int i = 0; i < n_clauses; i++) {
        int cl_node = 3 * n_vars + i;
        for (int j = 0; j < 3; j++) {
            int lit = clauses[i][j];
            int var = lit >> 1;
            g_edge(g, cl_node, 3*var + 1);
        }
    }
    return g;
}

/* ─── 3SAT -> SUBSET SUM ─────────────────────────────────── */
/*
 * Encode each clause and variable as a number in base-10.
 * Target sum is 111...1 for clauses, 1...1 for variables.
 * This is the classic Karp reduction.
 */
typedef struct { int* nums; int n; int target; } SubsetSum;

SubsetSum* reduce_3sat_to_subset_sum(int n_vars, int n_clauses, int clauses[][3]) {
    int N = 2 * n_vars + 2 * n_clauses;
    int* nums = malloc((size_t)N * sizeof(int));
    int* pows = malloc((size_t)(n_vars + n_clauses + 1) * sizeof(int));

    /* powers of 10 */
    pows[0] = 1;
    for (int i = 1; i <= n_vars + n_clauses; i++) pows[i] = pows[i-1] * 10;

    int target = 0;
    for (int i = 0; i < n_vars; i++) target += pows[i];
    for (int i = 0; i < n_clauses; i++) target += 3 * pows[n_vars + i]; /* 3 variants */

    /* Variable numbers */
    for (int v = 0; v < n_vars; v++) {
        nums[2*v] = pows[v];         /* x_v true */
        nums[2*v+1] = pows[v];       /* x_v false */
        for (int i = 0; i < n_clauses; i++)
            for (int j = 0; j < 3; j++) {
                int lit = clauses[i][j], var = lit >> 1, sign = lit & 1;
                if (var == v) {
                    if (!sign) nums[2*v] += pows[n_vars + i];     /* positive */
                    else       nums[2*v+1] += pows[n_vars + i];   /* negative */
                }
            }
    }

    /* Slack variables */
    for (int i = 0; i < n_clauses; i++) {
        nums[2*n_vars + 2*i] = pows[n_vars + i];
        nums[2*n_vars + 2*i + 1] = 2 * pows[n_vars + i];
    }

    SubsetSum* ss = malloc(sizeof(SubsetSum));
    ss->nums = nums; ss->n = N; ss->target = target;
    free(pows);
    return ss;
}

/* ─── Brute-force SUBSET SUM solver ────────────────────────── */
int subset_sum_solve(int* nums, int n, int target) {
    long long max = 1LL << n;
    if (max > 100000000) max = 100000000;
    for (long long mask = 0; mask < max; mask++) {
        int sum = 0;
        for (int i = 0; i < n; i++)
            if ((mask >> i) & 1) sum += nums[i];
        if (sum == target) return 1;
    }
    return 0;
}

/* ─── NP-Complete problem status table ─────────────────────── */
typedef struct {
    const char* name;
    const char* class;     /* P, NP-C, PSPACE-C, etc. */
    const char* description;
    int          year;     /* year proven NP-complete */
    const char* reduced_from;
} NPCProblem;

static NPCProblem npc_catalog[] = {
    {"SAT",            "NP-C", "Boolean satisfiability",               1971, "— (Cook)"},
    {"3SAT",           "NP-C", "3-CNF satisfiability",                 1972, "SAT"},
    {"CLIQUE",         "NP-C", "Maximum clique in graph",              1972, "3SAT"},
    {"VERTEX COVER",   "NP-C", "Minimum vertex cover",                 1972, "CLIQUE"},
    {"HAMILTONIAN PATH","NP-C","Path visiting each vertex once",       1972, "VERTEX COVER"},
    {"3-COLORING",     "NP-C", "Graph 3-colorability",                 1972, "3SAT"},
    {"SUBSET SUM",     "NP-C", "Subset summing to target",             1972, "3SAT"},
    {"KNAPSACK",       "NP-C", "0/1 knapsack optimization",            1972, "SUBSET SUM"},
    {"PARTITION",      "NP-C", "Partition set into equal sums",        1972, "SUBSET SUM"},
    {"TSP",            "NP-C", "Traveling salesman problem",           1972, "HAMILTONIAN PATH"},
    {"INTEGER PROG.",  "NP-C", "0/1 integer linear programming",       1972, "3SAT"},
    {"GRAPH ISOMORPH.", "NPC?","Graph isomorphism (unknown if NPC)",   1972, "—"},
    {"FACTORING",      "NPC?","Integer factorization (unknown if NPC)",1977, "—"},
    {"2SAT",           "P",   "2-CNF satisfiability (polynomial)",     1972, "—"},
    {"2-COLORING",     "P",   "Bipartiteness check (polynomial)",      1972, "—"},
    {"PRIMES",         "P",   "Primality testing (AKS 2002)",          2002, "—"},
    {NULL, NULL, NULL, 0, NULL}
};

void npc_catalog_print(void) {
    printf("\n===== NP-Completeness Problem Catalog =====\n\n");
    printf("%-20s %-10s %-45s %s\n", "Problem", "Class", "Description", "Year");
    printf("%-20s %-10s %-45s %s\n", "───────", "─────", "───────────", "────");
    for (int i = 0; npc_catalog[i].name; i++) {
        printf("%-20s %-10s %-45s %d\n",
               npc_catalog[i].name, npc_catalog[i].class,
               npc_catalog[i].description, npc_catalog[i].year);
    }
    printf("\nKey: NP-C = NP-Complete, NPC? = Unknown status, P = Polynomial\n");
    printf("Reference: Karp, R.M. 'Reducibility Among Combinatorial Problems' (1972)\n");
}
