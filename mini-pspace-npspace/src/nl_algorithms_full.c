/* nl_algorithms_full.c — NL-complete algorithms + logspace demonstrations
 * PATH: directed graph reachability (NL-complete)
 * 2-SAT: in NL (implication graph reachability)
 * Bipartiteness: in NL (2-coloring via reachability) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_V 128

typedef struct { int n; int** adj; } Graph;

Graph* g_new(int n){Graph*g=malloc(sizeof(Graph));g->n=n;g->adj=malloc((size_t)n*sizeof(int*));for(int i=0;i<n;i++)g->adj[i]=calloc((size_t)n,sizeof(int));return g;}
void g_edge(Graph*g,int u,int v){g->adj[u][v]=1;}
void g_free(Graph*g){for(int i=0;i<g->n;i++)free(g->adj[i]);free(g->adj);free(g);}

/* BFS reachability in O(n+m) time (P, not NL) */
int path_bfs(Graph* g, int s, int t) {
    int*vis=calloc((size_t)g->n,sizeof(int));int*q=malloc((size_t)g->n*sizeof(int));int qh=0,qt=0;
    q[qt++]=s;vis[s]=1;
    while(qh<qt){int u=q[qh++];if(u==t){free(vis);free(q);return 1;}
        for(int v=0;v<g->n;v++)if(g->adj[u][v]&&!vis[v]){vis[v]=1;q[qt++]=v;}}
    free(vis);free(q);return 0;
}

/* NL reachability: nondeterministic guess of path (simulated deterministically) */
int path_nl(Graph* g, int s, int t, int max_steps) {
    int current = s;
    for(int step=0;step<max_steps;step++){
        if(current==t) return 1;
        int choices[MAX_V], nc=0;
        for(int v=0;v<g->n;v++) if(g->adj[current][v]) choices[nc++]=v;
        if(nc==0) return 0;
        current = choices[rand()%nc];
    }
    return 0;
}

void nl_algorithms_demo(void) {
    printf("\n===== NL Algorithms =====\n\n");
    Graph* g = g_new(5);
    g_edge(g,0,1);g_edge(g,1,2);g_edge(g,2,3);g_edge(g,1,4);g_edge(g,4,3);
    
    printf("5-node digraph: 0->1, 1->2, 2->3, 1->4, 4->3\n");
    printf("  PATH(0,3) BFS = %d\n", path_bfs(g,0,3));
    printf("  PATH(3,0) BFS = %d\n", path_bfs(g,3,0));
    
    printf("\nPATH is NL-complete.\n");
    printf("NL algorithm: guess next vertex, keep only current + counter (O(log n) space).\n");
    printf("BFS needs O(n) space — polynomial but not logspace.\n");
    g_free(g);
}