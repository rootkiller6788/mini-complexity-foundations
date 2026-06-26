/* hex_game.c — Hex: PSPACE-complete board game
 * Two players (Red/Blue) alternate placing stones on a hexagonal grid.
 * Red wins by connecting top to bottom; Blue connects left to right.
 * Deciding which player has a winning strategy is PSPACE-complete.
 * First-player always wins (strategy stealing), but finding the strategy
 * is PSPACE-hard. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEX_SIZE 5

typedef struct { char board[HEX_SIZE][HEX_SIZE]; int turn; } HexState;

static void hex_init(HexState* s) {
    memset(s->board, '.', sizeof(s->board));
    s->turn = 0; /* 0=Red, 1=Blue */
}

static int hex_has_path(HexState* s, int player, int visited[HEX_SIZE][HEX_SIZE], int r, int c) {
    if(player==0 && r==HEX_SIZE-1) return 1; /* Red: reached bottom */
    if(player==1 && c==HEX_SIZE-1) return 1; /* Blue: reached right */
    visited[r][c] = 1;
    /* Hexagonal neighbors: 6 directions */
    int dr[] = {-1,-1,0,0,1,1}, dc[] = {0,1,-1,1,-1,0};
    for(int d=0;d<6;d++) {
        int nr=r+dr[d], nc=c+dc[d];
        if(nr>=0&&nr<HEX_SIZE&&nc>=0&&nc<HEX_SIZE&&!visited[nr][nc]&&s->board[nr][nc]==(player?'B':'R'))
            if(hex_has_path(s,player,visited,nr,nc)) return 1;
    }
    return 0;
}

static int hex_winner(HexState* s) {
    int visited[HEX_SIZE][HEX_SIZE];
    for(int c=0;c<HEX_SIZE;c++) {
        if(s->board[0][c]=='R') { memset(visited,0,sizeof(visited)); if(hex_has_path(s,0,visited,0,c)) return 0; }
    }
    for(int r=0;r<HEX_SIZE;r++) {
        if(s->board[r][0]=='B') { memset(visited,0,sizeof(visited)); if(hex_has_path(s,1,visited,r,0)) return 1; }
    }
    return -1;
}

static void hex_print(HexState* s) {
    for(int r=0;r<HEX_SIZE;r++) {
        for(int sp=0;sp<r;sp++) printf(" ");
        for(int c=0;c<HEX_SIZE;c++) printf("%c ", s->board[r][c]);
        printf("\n");
    }
}

void hex_demo(void) {
    printf("\n===== Hex: PSPACE-Complete Game =====\n\n");
    printf("Hex board (%dx%d). Red: top->bottom. Blue: left->right.\n", HEX_SIZE, HEX_SIZE);
    printf("PSPACE-complete: deciding winner of an arbitrary position.\n");
    printf("First player always wins (Nash 1947, strategy stealing).\n\n");
    
    HexState s; hex_init(&s);
    s.board[0][1]='R'; s.board[1][1]='R'; s.board[2][1]='R';
    s.board[3][2]='R'; s.board[4][2]='R';
    s.board[1][0]='B'; s.board[2][0]='B'; s.board[3][0]='B';
    
    printf("Example position:\n");
    hex_print(&s);
    int w = hex_winner(&s);
    printf("Winner: %s\n", w==0?"Red":w==1?"Blue":"none");
    printf("\nFinding winning strategy = PSPACE = O(board_size) space.\n");
}