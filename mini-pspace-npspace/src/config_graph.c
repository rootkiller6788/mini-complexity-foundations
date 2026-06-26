/* config_graph.c — Configuration graph for space-bounded computation
 * Build the directed graph of TM configurations reachable within space bound.
 * Number of configs = |Q| * s * |Gamma|^s = exponential in s.
 * PATH in this graph = acceptance. Illustrates Savitch's theorem. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int state; int head; int* tape; int tape_len; } Config;

static int config_eq(Config* a, Config* b) {
    if(a->state!=b->state || a->head!=b->head) return 0;
    for(int i=0;i<a->tape_len;i++) if(a->tape[i]!=b->tape[i]) return 0;
    return 1;
}

static int config_hash(Config* c) {
    int h=c->state*1007+c->head*31;
    for(int i=0;i<c->tape_len;i++) h=h*31+c->tape[i];
    return h<0?-h:h;
}

void config_graph_demo(void) {
    printf("\n===== Configuration Graph =====\n\n");
    printf("For a TM using s(n) space, the configuration graph has:\n");
    printf("  Vertices: |Q| * s(n) * |Gamma|^{s(n)} (exponential!)\n");
    printf("  Edges: each config has <= |Delta| outgoing edges\n");
    printf("  PATH(start, accept) = acceptance problem\n\n");
    printf("Savitch solves PATH in SPACE(log^2 |V|) = SPACE(s^2).\n");
    printf("This is why NPSPACE = PSPACE: the PATH algorithm works\n");
    printf("deterministically using O(s^2) space.\n");
    printf("The exponential number of configs requires O(s) bits\n");
    printf("to represent, but O(s^2) to solve PATH recursively.\n");
}