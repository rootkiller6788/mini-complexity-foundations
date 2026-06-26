/* nl_reachability.c — NL graph reachability (PATH problem)
 * PATH: given directed graph G and nodes s,t, is there a path s->t?
 * NL algorithm: guess next vertex nondeterministically, maintain
 * step counter (logspace). Total space = O(log n). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NL_MAX 32

typedef struct { int n; char adj[NL_MAX][NL_MAX]; } NLGraph;

static NLGraph* nlg_create(int n) {
    NLGraph* g=malloc(sizeof(NLGraph)); g->n=n; memset(g->adj,0,sizeof(g->adj)); return g;
}

static void nlg_edge(NLGraph* g,int u,int v){g->adj[u][v]=1;}

/* Nondeterministic PATH: guesses next vertex at each step.
   Determined by random seed for demonstration. Space = O(log n). */
int path_nl_sim(NLGraph* g, int s, int t, int max_steps, int seed) {
    srand(seed);
    int current = s, steps = 0;
    while(steps < max_steps) {
        if(current == t) return 1;
        int choices[NL_MAX], nc=0;
        for(int v=0;v<g->n;v++) if(g->adj[current][v]) choices[nc++]=v;
        if(nc==0) return 0;
        current = choices[rand()%nc]; steps++;
    }
    return 0;
}

/* Deterministic PATH via BFS: O(n+m) time, O(n) space (not logspace!) */
int path_det(NLGraph* g, int s, int t) {
    int vis[NL_MAX]={0}, q[NL_MAX], qh=0, qt=0;
    q[qt++]=s; vis[s]=1;
    while(qh<qt){int u=q[qh++];if(u==t)return 1;
        for(int v=0;v<g->n;v++)if(g->adj[u][v]&&!vis[v]){vis[v]=1;q[qt++]=v;}}
    return 0;
}

void nl_reachability_demo(void) {
    printf("\n===== NL Reachability (PATH) =====\n\n");
    NLGraph* g=nlg_create(8);
    nlg_edge(g,0,1);nlg_edge(g,1,2);nlg_edge(g,2,3);nlg_edge(g,3,4);
    nlg_edge(g,1,5);nlg_edge(g,5,6);nlg_edge(g,6,4);
    nlg_edge(g,4,7);
    
    printf("Graph: 0->1->2->3->4, 1->5->6->4, 4->7\n");
    printf("  Deterministic PATH(0,7): %d\n", path_det(g,0,7));
    printf("  Deterministic PATH(7,0): %d\n", path_det(g,7,0));
    printf("  NL PATH(0,7) seed=42: %d\n", path_nl_sim(g,0,7,20,42));
    printf("  NL PATH(0,7) seed=99: %d\n", path_nl_sim(g,0,7,20,99));
    
    printf("\nDeterministic PATH uses O(n) space (BFS queue).\n");
    printf("NL PATH uses O(log n) space (current+count only).\n");
    printf("Open: can PATH be solved in deterministic logspace?\n");
    free(g);
}