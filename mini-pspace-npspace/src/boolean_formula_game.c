/* boolean_formula_game.c — Boolean Formula Game (PSPACE-complete)
 * Two players (E/A) take turns assigning truth values to variables.
 * E wins if phi evaluates to TRUE; A wins if FALSE.
 * Deciding winner = deciding truth of QBF. PSPACE-complete.
 * This is the game-theoretic view of TQBF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int* assign; int n; int turn; int (*phi)(int*); } BFormulaGame;

static int example_phi(int* a) {
    /* (x0|x1) & (!x0|!x1) — XOR */
    return ((a[0]||a[1]) && (!a[0]||!a[1]));
}

static int bf_has_win(BFormulaGame* g, int var, int (*phi)(int*)) {
    if(var == g->n) return phi(g->assign);
    int saved = g->assign[var];
    g->assign[var]=0; int r0 = bf_has_win(g, var+1, phi);
    g->assign[var]=1; int r1 = bf_has_win(g, var+1, phi);
    g->assign[var]=saved;
    return (g->turn==0) ? (r0||r1) : (r0&&r1);
}

void bf_game_demo(void) {
    printf("\n===== Boolean Formula Game (PSPACE) =====\n\n");
    printf("Two players assign variables turn by turn.\n");
    printf("E wins if formula TRUE, A wins if FALSE.\n");
    printf("Deciding winner = TQBF = PSPACE-complete.\n\n");
    
    BFormulaGame g; g.n=2; g.turn=0;
    g.assign=calloc(2,sizeof(int));
    int e_wins = bf_has_win(&g, 0, example_phi);
    printf("Formula: (x0|x1)&(!x0|!x1)  E plays first.\n");
    printf("E has winning strategy: %s\n", e_wins?"YES":"NO");
    free(g.assign);
    
    printf("\nThis is exactly the TQBF problem in game form.\n");
    printf("Each quantifier = one player turn.\n");
}