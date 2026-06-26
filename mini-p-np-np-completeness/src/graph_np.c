/* graph_np.c — NP-Complete Graph Problem Solvers
 *
 * Implements exponential-time algorithms for classic NP-complete
 * graph problems: CLIQUE, VERTEX-COVER, 3-COLORING, INDEPENDENT-SET,
 * SUBGRAPH-ISOMORPHISM. Uses the Graph type from np_catalog.h.
 *
 * Knowledge:
 *   L1: CLIQUE, VERTEX-COVER, INDEPENDENT-SET, 3-COLORING definitions
 *   L2: Search-to-decision reduction (polynomial equivalence)
 *   L4: Karp reductions: CLIQUE ≤p VC, IS ≤p CLIQUE
 *   L5: Branch-and-bound with heuristic pruning
 *   L6: All problems are NP-complete (Karp 1972)
 *   L7: Graph coloring → register allocation, clique → social network analysis
 *
 * Reference: Garey & Johnson (1979), Karp (1972)
 */
#include "types.h"
#include "np_catalog.h"  /* for Graph, g_new(), g_free() */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int clique_found = 0;
static int* best_clique = NULL;

static void clique_search(const Graph* g, int* current, int cur_sz,
                          int start, int k_target) {
    if (clique_found) return;
    if (cur_sz == k_target) {
        clique_found = 1;
        memcpy(best_clique, current, (size_t)k_target * sizeof(int));
        return;
    }
    if (cur_sz + (g->n - start) < k_target) return;
    for (int v = start; v < g->n; v++) {
        int is_connected = 1;
        for (int i = 0; i < cur_sz; i++)
            if (!g->adj[current[i]][v]) { is_connected = 0; break; }
        if (is_connected) {
            current[cur_sz] = v;
            clique_search(g, current, cur_sz + 1, v + 1, k_target);
        }
    }
}

int clique_solve(const Graph* g, int k, int* result, int max_k) {
    clique_found = 0;
    best_clique = (int*)malloc((size_t)k * sizeof(int));
    int* current = (int*)malloc((size_t)k * sizeof(int));
    clique_search(g, current, 0, 0, k);
    if (clique_found && result) {
        int n = (k < max_k) ? k : max_k;
        memcpy(result, best_clique, (size_t)n * sizeof(int));
    }
    free(current); free(best_clique);
    return clique_found;
}

Graph* graph_complement(const Graph* g) {
    int n = g->n; Graph* c = g_new(n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (i != j) c->adj[i][j] = !g->adj[i][j];
    return c;
}

int vertex_cover_via_clique(const Graph* g, int k) {
    int n = g->n; Graph* comp = graph_complement(g);
    int result = clique_solve(comp, n - k, NULL, 0);
    g_free(comp); return result;
}

static int color_ok(const Graph* g, int* colors, int v, int c) {
    for (int u = 0; u < g->n; u++)
        if (g->adj[v][u] && colors[u] == c) return 0;
    return 1;
}

static int coloring_search(const Graph* g, int* colors, int v, int max_colors) {
    if (v == g->n) return 1;
    for (int c = 0; c < max_colors; c++) {
        if (color_ok(g, colors, v, c)) {
            colors[v] = c;
            if (coloring_search(g, colors, v + 1, max_colors)) return 1;
            colors[v] = -1;
        }
    }
    return 0;
}

int graph_k_colorable(const Graph* g, int k) {
    int n = g->n;
    int* colors = (int*)malloc((size_t)n * sizeof(int));
    for (int i = 0; i < n; i++) colors[i] = -1;
    int result = coloring_search(g, colors, 0, k);
    free(colors); return result;
}

int independent_set_solve(const Graph* g, int k) {
    Graph* comp = graph_complement(g);
    int result = clique_solve(comp, k, NULL, 0);
    g_free(comp); return result;
}

static int iso_found = 0;

static void iso_search(const Graph* g, const Graph* h,
                       int* mapping, int mapped_count, int* used_g) {
    if (iso_found) return;
    if (mapped_count == h->n) {
        int ok = 1;
        for (int i = 0; i < h->n && ok; i++)
            for (int j = 0; j < h->n && ok; j++)
                if (h->adj[i][j] && !g->adj[mapping[i]][mapping[j]]) ok = 0;
        if (ok) iso_found = 1;
        return;
    }
    for (int v = 0; v < g->n; v++) {
        if (used_g[v]) continue;
        mapping[mapped_count] = v; used_g[v] = 1;
        iso_search(g, h, mapping, mapped_count + 1, used_g);
        used_g[v] = 0; if (iso_found) return;
    }
}

int subgraph_isomorphism(const Graph* g, const Graph* h) {
    iso_found = 0;
    int* mapping = (int*)malloc((size_t)h->n * sizeof(int));
    int* used_g  = (int*)calloc((size_t)g->n, sizeof(int));
    iso_search(g, h, mapping, 0, used_g);
    free(mapping); free(used_g); return iso_found;
}

Graph* reduce_3sat_to_clique_graph(int n_vars, int n_clauses, int clauses[][3]) {
    int V = 3 * n_clauses; Graph* g = g_new(V);
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

int sat_via_clique(int n_vars, int n_clauses, int clauses[][3]) {
    Graph* g = reduce_3sat_to_clique_graph(n_vars, n_clauses, clauses);
    int result = clique_solve(g, n_clauses, NULL, 0);
    g_free(g); return result;
}
