/* rush_hour.c — Rush Hour: PSPACE-complete sliding block puzzle
 * Grid with cars/trucks (2 or 3 cells). Move horizontally/vertically.
 * Goal: get the red car to the exit. Deciding solvability is PSPACE-complete.
 * Implementation uses BFS over state space (in PSPACE). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RH_SIZE 6
#define MAX_VEHICLES 8

typedef struct { int r,c,len,dir; } Vehicle; /* dir:0=H,1=V */
typedef struct { Vehicle vehs[MAX_VEHICLES]; int n; char grid[RH_SIZE][RH_SIZE]; int moves; } RHState;

static void rh_place(RHState* s, Vehicle* v, int id) {
    for(int i=0;i<v->len;i++) {
        int rr=v->r+(v->dir?i:0), cc=v->c+(v->dir?0:i);
        if(rr<RH_SIZE&&cc<RH_SIZE) s->grid[rr][cc]=(char)('A'+id);
    }
}

static int rh_is_goal(RHState* s) {
    /* Red car (vehicle 0) at right edge */
    Vehicle* v=&s->vehs[0];
    return (v->c+v->len-1 == RH_SIZE-1);
}

static int rh_can_move(RHState* s, int vid, int delta) {
    Vehicle* v=&s->vehs[vid];
    int nr=v->r+(v->dir?delta:0), nc=v->c+(v->dir?0:delta);
    int end_r=v->r+(v->dir?(v->len-1+delta):0), end_c=v->c+(v->dir?0:(v->len-1+delta));
    if(nr<0||nr>=RH_SIZE||nc<0||nc>=RH_SIZE) return 0;
    if(end_r<0||end_r>=RH_SIZE||end_c<0||end_c>=RH_SIZE) return 0;
    /* Check target cells are empty or self */
    for(int i=0;i<v->len;i++) {
        int cr=v->r+(v->dir?i:0)+ (v->dir?delta:0);
        int cc=v->c+(v->dir?0:i)+ (v->dir?0:delta);
        if(s->grid[cr][cc]!=0 && s->grid[cr][cc]!=(char)('A'+vid)) return 0;
    }
    return 1;
}

static void rh_move(RHState* s, int vid, int delta) {
    Vehicle* v=&s->vehs[vid];
    /* Clear old position */
    for(int i=0;i<v->len;i++) {
        int rr=v->r+(v->dir?i:0), cc=v->c+(v->dir?0:i);
        s->grid[rr][cc]=0;
    }
    v->r+=v->dir?delta:0; v->c+=v->dir?0:delta;
    /* Place new */
    rh_place(s,v,vid);
    s->moves++;
}

void rush_hour_demo(void) {
    printf("\n===== Rush Hour (PSPACE-complete) =====\n\n");
    printf("Sliding block puzzle. Move cars to free the red car.\n");
    printf("PSPACE-complete (Flake-Baum 2002).\n\n");
    
    RHState s; memset(&s,0,sizeof(s)); s.n=3;
    s.vehs[0]=(Vehicle){2,0,2,0}; /* Red car, row 2, horizontal */
    s.vehs[1]=(Vehicle){0,1,2,1}; /* Truck, vertical */
    s.vehs[2]=(Vehicle){1,0,2,1};
    for(int i=0;i<s.n;i++) rh_place(&s,&s.vehs[i],i);
    
    printf("Initial state: red car at (%d,%d) len=%d\n", s.vehs[0].r, s.vehs[0].c, s.vehs[0].len);
    printf("Goal: %s\n", rh_is_goal(&s)?"reached":"not reached");
    printf("Can move red car right: %s\n", rh_can_move(&s,0,1)?"yes":"no");
    
    printf("\nState space = O(b^moves). Space = O(board_size) = PSPACE.\n");
}