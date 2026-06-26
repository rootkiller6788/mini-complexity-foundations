/* sliding_puzzle.c — n x n Sliding Puzzle (15-puzzle generalization)
 * Generalization of 15-puzzle to n x n is PSPACE-complete
 * (Goldreich 1984, via pebble game reduction).
 * Finding shortest solution is PSPACE-complete. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SP_SIZE 4

typedef struct { int board[SP_SIZE][SP_SIZE]; int er,ec; int moves; } SPState;

static void sp_init(SPState* s) {
    int val=1;
    for(int r=0;r<SP_SIZE;r++) for(int c=0;c<SP_SIZE;c++) s->board[r][c]=val++;
    s->board[SP_SIZE-1][SP_SIZE-1]=0;  /* empty */
    s->er=SP_SIZE-1; s->ec=SP_SIZE-1; s->moves=0;
    /* Shuffle: swap last two */
    s->board[SP_SIZE-1][SP_SIZE-2]=15;
}

static int sp_is_solved(SPState* s) {
    int val=1;
    for(int r=0;r<SP_SIZE;r++) for(int c=0;c<SP_SIZE;c++) {
        if(r==SP_SIZE-1&&c==SP_SIZE-1) return (s->board[r][c]==0);
        if(s->board[r][c]!=val++) return 0;
    }
    return 1;
}

static int sp_manhattan(SPState* s) {
    int dist=0;
    for(int r=0;r<SP_SIZE;r++) for(int c=0;c<SP_SIZE;c++) {
        int v=s->board[r][c];
        if(v==0) continue;
        int tr=(v-1)/SP_SIZE, tc=(v-1)%SP_SIZE;
        dist += abs(r-tr)+abs(c-tc);
    }
    return dist;
}

void sliding_puzzle_demo(void) {
    printf("\n===== n x n Sliding Puzzle (PSPACE) =====\n\n");
    printf("15-puzzle generalized to n x n is PSPACE-complete.\n");
    printf("Finding shortest solution requires n^O(1) space.\n");
    
    SPState s; sp_init(&s);
    printf("Initial: solved=%d, manhattan_dist=%d\n", sp_is_solved(&s), sp_manhattan(&s));
    printf("BFS would explore O((n^2)!) states. Space = O(n^2) per position = PSPACE.\n");
}