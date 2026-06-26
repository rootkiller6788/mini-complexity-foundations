/* reachability_method.c — 到達可能性法: グラフ探索の空間計算量
 * 参考: Wigderson 1992, "The complexity of graph connectivity"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/spaceh.h"

/* Reachability in various graph classes — a unifying theme in space complexity
 * STCON: directed s-t connectivity → NL-complete
 * USTCON: undirected s-t connectivity → L (Reingold 2008!)
 * 1-STCON: out-degree ≤ 1 → RL
 * Tree-STCON: tree reachability → L
 */

/* Savitch: NSPACE(S) ⊆ DSPACE(S²) via reachability */
typedef struct {
    int from;
    int to;
    int length;
} path_config;

/* Savitch recursion: PATH(u,v,k) iff can go u->v in <= 2^k steps
 * Space: O(k * log n) = O(log^2 n)
 * Internal helper with visited array and depth tracking. */
static int savitch_rec_visited(int** graph, int n, int u, int v, int k,
                                int* visited, int* stack_depth) {
    (*stack_depth)++;
    if (*stack_depth > 100) { (*stack_depth)--; return 0; } /* safety */

    if (k == 0) return (u == v || graph[u][v]);

    for (int w = 0; w < n; w++) {
        if (visited[w]) continue;
        visited[w] = 1;
        if (savitch_rec_visited(graph, n, u, w, k-1, visited, stack_depth) &&
            savitch_rec_visited(graph, n, w, v, k-1, visited, stack_depth)) {
            visited[w] = 0;
            (*stack_depth)--;
            return 1;
        }
        visited[w] = 0;
    }
    (*stack_depth)--;
    return 0;
}

/* Immerman-Szelepcsényi: NL = co-NL via inductive counting */
int reachable_count(int** graph, int n, int s, int d) {
    /* Count vertices reachable from s in ≤ d steps (in logspace!) */
    int count = 0;
    for (int v = 0; v < n; v++) {
        /* Check reachability in logspace via nondeterministic search */
        /* Simplified: call Savitch for verification */
        int* visited = calloc(n, sizeof(int));
        int depth = 0;
        if (savitch_rec_visited(graph, n, s, v, (int)(log2(d)+1), visited, &depth)) {
            count++;
        }
        free(visited);
    }
    return count;
}

/* Logspace graph exploration: O(log n) bits for pointer + O(1) registers */
void logspace_dfs_demo(int n) {
    printf("=== Logspace Graph Exploration ===\n\n");
    printf("Graph size n=%d\n", n);
    printf("Space used:\n");
    printf("  Vertex pointer:  log₂(%d) = %.1f bits\n", n, log2(n));
    printf("  Edge counter:    log₂(%d) = %.1f bits\n", n*n, log2(n*n));
    printf("  Step counter:    O(log n) bits\n");
    printf("  Total: O(log n) workspace → in L\n\n");
    
    printf("Key results:\n");
    printf("  STCON (directed)        NL-complete  [Jones 1975]\n");
    printf("  USTCON (undirected)     ∈ L          [Reingold 2008]\n");
    printf("  1-STCON (degree≤1)      ∈ RL         [Aleliunas+ 1979]\n");
    printf("  Tree-STCON              ∈ L          [trivial]\n");
    printf("  Planar-STCON            ∈ L          [Datta+ 2010]\n");
}

void reachability_method_demo(void) {
    logspace_dfs_demo(1024);

    /* Simple 4-node graph reachability demo */
    int** g = malloc(4 * sizeof(int*));
    for (int i = 0; i < 4; i++) {
        g[i] = calloc(4, sizeof(int));
    }
    g[0][1] = g[1][2] = g[2][3] = 1;

    int* vis = calloc(4, sizeof(int));
    int depth = 0;
    printf("\nPath 0->3: %s (Savitch)\n",
           savitch_rec_visited(g, 4, 0, 3, 3, vis, &depth) ? "YES" : "NO");

    for (int i = 0; i < 4; i++) free(g[i]);
    free(g); free(vis);
}
