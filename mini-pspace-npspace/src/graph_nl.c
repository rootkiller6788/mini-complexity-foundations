/* graph_nl.c — NL graph algorithms: PATH, connectivity
 * Demonstrates polynomial-time graph algorithms for NL problems.
 * NL subset P: can simulate nondeterministic logspace in poly time. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int n; int** adj; } Graph;

Graph* graph_create(int n) {
    Graph* g = malloc(sizeof(Graph)); g->n = n;
    g->adj = malloc((size_t)n * sizeof(int*));
    for(int i=0;i<n;i++) { g->adj[i] = calloc((size_t)n, sizeof(int)); }
    return g;
}

void graph_add_edge(Graph* g, int u, int v) { g->adj[u][v] = 1; }

/* BFS reachability: O(n+m) time, O(n) space — in P */
int bfs_reachable(Graph* g, int s, int t) {
    int* visited = calloc((size_t)g->n, sizeof(int));
    int* queue = malloc((size_t)g->n * sizeof(int));
    int qh=0, qt=0;
    queue[qt++] = s; visited[s] = 1;
    while(qh < qt) {
        int u = queue[qh++];
        if(u == t) { free(visited); free(queue); return 1; }
        for(int v=0; v<g->n; v++)
            if(g->adj[u][v] && !visited[v]) { visited[v]=1; queue[qt++]=v; }
    }
    free(visited); free(queue);
    return 0;
}

void graph_nl_demo(void) {
    printf("\n===== NL Graph Algorithms =====\n\n");
    Graph* g = graph_create(5);
    graph_add_edge(g,0,1); graph_add_edge(g,1,2);
    graph_add_edge(g,2,3); graph_add_edge(g,1,3);
    graph_add_edge(g,3,4);
    
    printf("Graph: 5 nodes, edges: 0->1, 1->2, 2->3, 1->3, 3->4\n");
    printf("  PATH(0,4) = %d (expected 1)\n", bfs_reachable(g,0,4));
    printf("  PATH(4,0) = %d (expected 0)\n", bfs_reachable(g,4,0));
    printf("\nPATH is NL-complete (nondeterministic logspace).\n");
    printf("BFS solves it in P (full graph storage = O(n) space > O(log n)).\n");
    printf("But an NL algorithm exists using only O(log n) space.\n");
    free(g);
}