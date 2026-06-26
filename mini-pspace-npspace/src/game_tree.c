/* game_tree.c — Min-Max game tree solver for PSPACE games
 * Two-player zero-sum games with polynomial depth are in PSPACE.
 * The game tree search uses O(depth) space = polynomial. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int* moves; int n_moves; int* next_state; int is_terminal; int winner; } GameState;

int minimax(GameState* states, int current, int depth, int max_depth, int maximizing) {
    GameState* s = &states[current];
    if(s->is_terminal) return s->winner == (maximizing?1:2) ? 1 : -1;
    if(depth >= max_depth) return 0; /* draw */
    int best = maximizing ? -999 : 999;
    for(int i=0;i<s->n_moves;i++) {
        int val = minimax(states, s->next_state[i], depth+1, max_depth, !maximizing);
        if(maximizing) { if(val > best) best = val; }
        else { if(val < best) best = val; }
    }
    return best;
}

void game_tree_demo(void) {
    printf("\n===== PSPACE Game Tree Search =====\n\n");
    printf("Two-player zero-sum games with poly-depth are in PSPACE.\n");
    printf("Space: O(depth) to store recursion stack.\n");
    printf("Each state can be stored in polynomial space.\n");
    printf("This is why Generalized Geography is PSPACE-complete.\n\n");
    printf("Minimax value = 1 (Player 1 wins), -1 (Player 2 wins), 0 (draw).\n");
}