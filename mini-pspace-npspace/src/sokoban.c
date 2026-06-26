/* sokoban.c — Sokoban: PSPACE-complete warehouse puzzle
 * Push boxes to target positions. Deciding solvability = PSPACE-complete.
 * Culberson (1997) proved this via reduction from Nondeterministic
 * Constraint Logic (Hearn-Demaine). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SK_SIZE 8

typedef struct { char grid[SK_SIZE][SK_SIZE]; int px,py; int boxes; int targets; int moves; } SokoState;

static void sk_init(SokoState* s) {
    memset(s->grid, '.', sizeof(s->grid));
    /* Walls */
    for(int i=0;i<SK_SIZE;i++) { s->grid[i][0]='#'; s->grid[i][SK_SIZE-1]='#'; s->grid[0][i]='#'; s->grid[SK_SIZE-1][i]='#'; }
    /* Player, boxes, targets */
    s->px=2; s->py=3; s->grid[s->py][s->px]='@';
    s->grid[3][3]='$'; s->boxes=1;
    s->grid[4][5]='.'; s->grid[4][5]='*'; s->targets=0;
    s->grid[4][4]='+'; s->targets++;
    s->moves=0;
}

static int sk_is_win(SokoState* s) {
    for(int r=0;r<SK_SIZE;r++) for(int c=0;c<SK_SIZE;c++)
        if(s->grid[r][c]=='$') return 0;  /* box not on target */
    return 1;
}

void sokoban_demo(void) {
    printf("\n===== Sokoban (PSPACE-complete) =====\n\n");
    printf("Push boxes onto target squares. Player pulls.\n");
    printf("PSPACE-complete (Culberson 1997).\n\n");
    printf("Board: @=player, $=box, .=target, *=box on target, +=player on target\n");
    printf("Reachable state space = 2^O(n). Search = PSPACE.\n");
}