/* succinct_graphs.c — Succinct Graph Representations
 * A graph with 2^n vertices can be described by a Boolean circuit with
 * O(log n) input bits. PATH on succinct graphs is NPSPACE-complete,
 * which equals PSPACE by Savitch.
 * This connects circuit complexity to space complexity. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A succinct graph: vertices are n-bit strings.
 * Edge predicate E(u,v) is computed by a Boolean circuit.
 * For path problems, we manipulate vertex IDs as bitstrings. */
typedef int (*EdgePred)(int u, int v);

static int grid_edge(int u, int v) {
    /* 2D grid: (r,c) -> (r-1,c), (r+1,c), (r,c-1), (r,c+1) */
    int r1=u>>8, c1=u&255, r2=v>>8, c2=v&255;
    return (abs(r1-r2)+abs(c1-c2)==1);
}

void succinct_graphs_demo(void) {
    printf("\n===== Succinct Graphs =====\n\n");
    printf("A graph with N=2^n vertices can be described by a poly-size circuit.\n");
    printf("PATH on succinct graphs is NPSPACE-complete = PSPACE-complete.\n");
    printf("This shows the connection: circuits + space complexity.\n\n");
    printf("Example: N=256 vertex grid. Edge predicate = O(log N) bits.\n");
    printf("PATH can be solved in SPACE(log^2 N) = polynomial.\n");
}