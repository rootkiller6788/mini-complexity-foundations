/* game_solver.c — Full minimax game solver for PSPACE-complete games
 * Generates random game graphs and solves them with alpha-beta pruning.
 * Time: O(b^d), Space: O(d) = polynomial. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NODES 100
#define MAX_DEPTH 20

typedef struct { int children[MAX_NODES]; int n_child; int value; int is_leaf; } Node;

static Node tree[MAX_NODES];
static int n_nodes;

static int build_game_tree(int depth, int max_depth, int branching) {
    int id = n_nodes++;
    if(depth >= max_depth) { tree[id].is_leaf=1; tree[id].value=rand()%3-1; return id; }
    tree[id].n_child = 1 + rand()%branching;
    for(int i=0;i<tree[id].n_child && n_nodes<MAX_NODES;i++)
        tree[id].children[i] = build_game_tree(depth+1, max_depth, branching);
    return id;
}

static int alpha_beta(int node, int depth, int alpha, int beta, int maximizing) {
    Node* n = &tree[node];
    if(n->is_leaf || depth==0) return n->value;
    if(maximizing) {
        int v = -999;
        for(int i=0;i<n->n_child;i++) {
            int cv = alpha_beta(n->children[i], depth-1, alpha, beta, 0);
            if(cv > v) v = cv;
            if(v > alpha) alpha = v;
            if(alpha >= beta) break;
        }
        return v;
    } else {
        int v = 999;
        for(int i=0;i<n->n_child;i++) {
            int cv = alpha_beta(n->children[i], depth-1, alpha, beta, 1);
            if(cv < v) v = cv;
            if(v < beta) beta = v;
            if(alpha >= beta) break;
        }
        return v;
    }
}

void game_solver_demo(void) {
    printf("\n===== PSPACE Game Solver =====\n\n");
    srand(42);
    printf("Generating random game tree with alpha-beta search:\n");
    printf("  depth  nodes  result  space(O(d))\n");
    printf("  -----  -----  ------  ----------\n");
    for(int d=1;d<=6;d++) {
        n_nodes = 0; memset(tree,0,sizeof(tree));
        build_game_tree(0,d,2+rand()%2);
        int result = alpha_beta(0,d,-999,999,1);
        printf("  %-6d %-6d %-7d %-10d\n", d, n_nodes, result, d*4);
    }
    printf("\nSpace = O(depth) to store recursion stack.\n");
    printf("This is why 2-player games with poly-depth are in PSPACE.\n");
}