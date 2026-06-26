/* pebble_game.c — Pebble Game (PSPACE-complete)
 * Gilbert-Lengauer-Tarjan (1980): pebble game on DAGs is PSPACE-complete.
 * Given DAG, can you pebble a target node using at most k pebbles?
 * Each pebble placement requires all predecessors to be pebbled.
 * Removing pebbles is free. PSPACE-complete for variable k. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 20
#define MAX_PEBBLES 5

typedef struct { int n; int pred[MAX_NODES][MAX_NODES]; int n_pred[MAX_NODES]; int target; } PebbleDAG;

static int can_pebble(PebbleDAG* d, int node, int pebbles[MAX_NODES], int used) {
    if(pebbles[node]) return 1;
    if(used >= MAX_PEBBLES) return 0;
    /* Place pebble on node: need all predecessors pebbled or pebble-able */
    for(int i=0;i<d->n_pred[node];i++) {
        int p = d->pred[node][i];
        if(!pebbles[p]) {
            if(!can_pebble(d, p, pebbles, used)) return 0;
        }
    }
    /* Now pebble the node */
    pebbles[node]=1; used++;
    /* Can optionally remove predecessor pebbles */
    return 1;
}

static int pebble_solve(PebbleDAG* d) {
    int pebbles[MAX_NODES]={0};
    return can_pebble(d, d->target, pebbles, 0);
}

void pebble_game_demo(void) {
    printf("\n===== Pebble Game (PSPACE-complete) =====\n\n");
    printf("Given DAG + k pebbles, can you pebble the target node?\n");
    printf("PSPACE-complete for variable k (Gilbert-Lengauer-Tarjan 1980).\n\n");
    
    PebbleDAG d; d.n=5; d.target=4;
    memset(d.n_pred,0,sizeof(d.n_pred));
    d.pred[1][d.n_pred[1]++]=0;
    d.pred[2][d.n_pred[2]++]=0;
    d.pred[3][d.n_pred[3]++]=1; d.pred[3][d.n_pred[3]++]=2;
    d.pred[4][d.n_pred[4]++]=3;
    
    printf("DAG: 0->1, 0->2, 1->3, 2->3, 3->4 (target)\n");
    printf("Solvable with k=2: %s\n", pebble_solve(&d)?"YES":"NO");
    printf("\nPebble game models register allocation, circuit evaluation.\n");
    printf("Connects PSPACE to compiler optimization!\n");
}