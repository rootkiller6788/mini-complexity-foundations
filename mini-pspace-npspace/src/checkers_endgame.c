/* checkers_endgame.c — Checkers Endgame: PSPACE-complete
 * Fraenkel et al. (1978): n x n checkers is PSPACE-complete.
 * We implement a small-board endgame solver with full minimax search.
 * Shows the space-bounded nature of game search. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CK_SIZE 4
#define MAX_MOVES 20

typedef struct { char board[CK_SIZE][CK_SIZE]; int turn; int pieces[2]; } CheckersState;

static void ck_init(CheckersState* s) {
    memset(s->board, '.', sizeof(s->board));
    /* Place pieces on small board */
    s->board[0][0]='X'; s->board[0][2]='X';  /* Player 0 (top) */
    s->board[3][1]='O'; s->board[3][3]='O';  /* Player 1 (bottom) */
    s->turn = 0; s->pieces[0]=2; s->pieces[1]=2;
}

static int ck_evaluate(CheckersState* s, int depth, int max_depth, long long* nodes) {
    (*nodes)++;
    if(depth >= max_depth) return 0;
    if(s->pieces[0]==0) return -100; /* Player 0 lost */
    if(s->pieces[1]==0) return 100;  /* Player 1 lost */
    
    int best = (s->turn==0) ? -999 : 999;
    int has_move = 0;
    char piece = (s->turn==0) ? 'X' : 'O';
    
    /* Try all moves for current player */
    for(int r=0;r<CK_SIZE;r++) for(int c=0;c<CK_SIZE;c++) {
        if(s->board[r][c] != piece) continue;
        int dr = (s->turn==0) ? 1 : -1;
        for(int dc=-1;dc<=1;dc+=2) {
            int nr=r+dr, nc=c+dc;
            if(nr>=0&&nr<CK_SIZE&&nc>=0&&nc<CK_SIZE) {
                if(s->board[nr][nc]=='.' || s->board[nr][nc]!=piece) {
                    has_move = 1;
                    char saved = s->board[nr][nc];
                    s->board[r][c]='.'; s->board[nr][nc]=piece;
                    int old_turn=s->turn; s->turn=1-old_turn;
                    int val = ck_evaluate(s, depth+1, max_depth, nodes);
                    s->turn=old_turn; s->board[r][c]=piece; s->board[nr][nc]=saved;
                    if(s->turn==0 && val>best) best=val;
                    if(s->turn==1 && val<best) best=val;
                }
            }
        }
    }
    return has_move ? best : 0;
}

void checkers_demo(void) {
    printf("\n===== Checkers Endgame (PSPACE) =====\n\n");
    printf("n x n checkers is PSPACE-complete (Fraenkel et al. 1978).\n");
    printf("Small board (%dx%d) with 2 pieces each:\n\n", CK_SIZE, CK_SIZE);
    
    CheckersState s; ck_init(&s);
    for(int r=0;r<CK_SIZE;r++) {
        for(int c=0;c<CK_SIZE;c++) printf("%c ", s.board[r][c]);
        printf("\n");
    }
    
    printf("\nSearching with depth limit:\n");
    printf("  depth  nodes     value\n");
    for(int d=1;d<=5;d++) {
        CheckersState sc = s;
        long long nodes = 0;
        int val = ck_evaluate(&sc, 0, d, &nodes);
        printf("  %-6d %-8lld %-6d\n", d, nodes, val);
    }
    printf("\nSpace = O(depth * board_size) = polynomial.\n");
    printf("Time = O(b^depth) = exponential. But in PSPACE!\n");
}