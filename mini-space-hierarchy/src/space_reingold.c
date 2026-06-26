/* space_reingold.c -- Reingold's Theorem (2008): USTCON in L
 *
 * Undirected s-t connectivity is in deterministic logspace!
 * This was a major breakthrough: SL = L.
 *
 * Key idea: Zig-zag graph product to convert any undirected
 * graph into an expander graph, then traverse the expander
 * in logspace.
 *
 * A complete implementation of Reingold's algorithm would require
 * expander graph constructions far beyond this module's scope.
 * We present the key conceptual pieces and a simplified demo.
 *
 * Reference: Reingold (STOC 2005, JACM 2008)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/spaceh.h"

/* ---------- Undirected Graph ---------- */

typedef struct {
    int n;
    int m;
    int** adj;   /* adjacency matrix, symmetric */
    int* deg;    /* degree of each vertex */
} UGraph;

static UGraph* ug_create(int n) {
    UGraph* g = malloc(sizeof(UGraph));
    g->n = n; g->m = 0;
    g->adj = malloc((size_t)n * sizeof(int*));
    g->deg = calloc((size_t)n, sizeof(int));
    for (int i = 0; i < n; i++)
        g->adj[i] = calloc((size_t)n, sizeof(int));
    return g;
}

static void ug_free(UGraph* g) {
    if (!g) return;
    for (int i = 0; i < g->n; i++) free(g->adj[i]);
    free(g->adj); free(g->deg); free(g);
}

static void ug_add_edge(UGraph* g, int u, int v) {
    if (!g->adj[u][v]) {
        g->adj[u][v] = g->adj[v][u] = 1;
        g->deg[u]++; g->deg[v]++;
        g->m++;
    }
}

/* ---------- Logspace USTCON ---------- */

/* Traditional BFS: O(n) space, NOT logspace.
 * Included as reference baseline. */
static int ustcon_bfs(UGraph* g, int s, int t) {
    int* visited = calloc((size_t)g->n, sizeof(int));
    int* queue = malloc((size_t)g->n * sizeof(int));
    int qh = 0, qt = 0, found = 0;
    queue[qt++] = s; visited[s] = 1;
    while (qh < qt && !found) {
        int u = queue[qh++];
        if (u == t) { found = 1; break; }
        for (int v = 0; v < g->n; v++)
            if (g->adj[u][v] && !visited[v])
                { visited[v] = 1; queue[qt++] = v; }
    }
    free(visited); free(queue);
    return found;
}

/* ---------- Zig-Zag Product (Conceptual) ---------- */

/* The zig-zag product G z H of a large graph G (N vertices, D-regular)
 * and a small graph H (D vertices, d-regular) produces a new graph
 * with N*D vertices that inherits expansion from H.
 *
 * Reingold's algorithm:
 * 1. Convert input graph into a regular graph (logspace)
 * 2. Apply zig-zag product repeatedly to boost expansion
 * 3. The resulting expander has diameter O(log N)
 * 4. Explore the expander exhaustively in O(log N) space!
 *
 * This is a simplified conceptual demonstration. */

/* Rotation map: for each vertex v and edge label i (0..D-1),
 * Rot[v][i] = (neighbor, return-label).
 * Used in zig-zag product construction. */
typedef struct {
    int neighbor;
    int return_label;
} RotationEntry;

/* Compute the spectral gap of a graph (conceptual).
 * For expander graphs: lambda < 1, giving O(log N) diameter. */
static double spectral_gap_estimate(UGraph* g) {
    /* Simplified: use degree-based estimate.
     * For a d-regular expander, lambda <= 2*sqrt(d-1)/d.
     * For non-regular graphs, this is just a heuristic. */
    double avg_deg = 0;
    for (int i = 0; i < g->n; i++) avg_deg += g->deg[i];
    avg_deg /= g->n;
    if (avg_deg <= 1.0) return 1.0;
    return 2.0 * sqrt(avg_deg - 1.0) / avg_deg;
}

/* ---------- Randomized Logspace USTCON ---------- */

/* Before Reingold, the best result was randomized logspace:
 * random walk on undirected graph reaches any connected
 * vertex in O(n^3) expected steps.
 * Space: O(log n) ¡ª only current vertex + step counter. */
static int ustcon_random_walk(UGraph* g, int s, int t,
                               int max_steps, unsigned int seed) {
    srand(seed);
    int cur = s, steps = 0;
    while (steps < max_steps) {
        if (cur == t) return 1;
        if (g->deg[cur] == 0) return 0;
        /* Pick random neighbor */
        int target = rand() % g->deg[cur];
        int found = 0, count = 0;
        for (int v = 0; v < g->n; v++) {
            if (g->adj[cur][v]) {
                if (count == target) { cur = v; found = 1; break; }
                count++;
            }
        }
        if (!found) return 0;
        steps++;
    }
    return 0;
}

/* ---------- Demo ---------- */

void reingold_theorem_demo(void) {
    printf("\n===== Reingold's Theorem: USTCON in L =====\n\n");

    printf("Reingold (2008): Undirected s-t connectivity is in L.\n");
    printf("This proves SL = L (Symmetric Logspace = Deterministic Logspace).\n\n");

    /* Build demo graph */
    UGraph* g = ug_create(10);
    /* Line + extra edges to make it non-trivial */
    for (int i = 0; i < 9; i++) ug_add_edge(g, i, i + 1);
    ug_add_edge(g, 0, 5);
    ug_add_edge(g, 3, 8);
    ug_add_edge(g, 2, 7);

    printf("--- Graph (10 vertices) ---\n");
    printf("  Line 0-1-2-3-4-5-6-7-8-9 + cross edges (0-5, 3-8, 2-7)\n");
    printf("  Edges: %d\n\n", g->m);

    printf("--- BFS Connectivity (reference, O(n) space) ---\n");
    for (int t = 5; t <= 9; t += 4)
        printf("  USTCON(0,%d) = %s\n", t,
               ustcon_bfs(g, 0, t) ? "YES" : "NO");

    printf("\n--- Random Walk (O(log n) space, but randomized) ---\n");
    for (int r = 0; r < 3; r++)
        printf("  RW USTCON(0,9) try %d = %s\n", r,
               ustcon_random_walk(g, 0, 9, 1000, (unsigned int)(123 + r))
               ? "YES" : "NO");

    printf("\n--- Spectral Gap Estimate ---\n");
    printf("  Estimated lambda: %.4f\n", spectral_gap_estimate(g));
    printf("  (For expanders, smaller lambda = better expansion)\n");

    printf("\n--- Reingold's Key Insight ---\n");
    printf("  1. Convert any undirected graph into a regular graph (logspace)\n");
    printf("  2. Use zig-zag product to build an expander\n");
    printf("  3. Expander has O(log N) diameter\n");
    printf("  4. Exhaustively explore in O(log N) space!\n\n");

    printf("--- Impact ---\n");
    printf("  SL = L: symmetric logspace = deterministic logspace.\n");
    printf("  Before 2008: best was randomized logspace (RL).\n");
    printf("  Reingold's result is fully deterministic!\n");
    printf("  Open: Does this extend to directed graphs (L vs NL)?\n");
    printf("  Conjecture: L = RL = BPL (all equal).\n");

    ug_free(g);
}
