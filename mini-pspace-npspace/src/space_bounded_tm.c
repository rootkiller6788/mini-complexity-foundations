/* space_bounded_tm.c — Space-bounded Turing Machine simulator
 * A TM using s(n) space can be simulated to test any s(n)-space property.
 * Configurations = (state, head_pos, tape_content). Count = O(|Q|*s*|Gamma|^s).
 * Key insight: a TM using polynomial space has exponentially many configs. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define S_MAX 20
#define S_TAPE 40

typedef struct { int state; int head; int tape[S_TAPE]; int n_state; int n_sym; } TMConfig;

typedef struct { int n_states; int n_syms; int q0, qa, qr; int* from_s; int* from_r; int* to_s; int* to_w; int* to_m; int n_trans; } SpaceTM;

static SpaceTM* stm_create(void) {
    SpaceTM* m = malloc(sizeof(SpaceTM));
    m->n_states=4; m->n_syms=2; m->q0=0; m->qa=2; m->qr=3; m->n_trans=6;
    m->from_s=malloc(16*sizeof(int)); m->from_r=malloc(16*sizeof(int));
    m->to_s=malloc(16*sizeof(int)); m->to_w=malloc(16*sizeof(int)); m->to_m=malloc(16*sizeof(int));
    int i=0;
    m->from_s[i]=0;m->from_r[i]=0;m->to_s[i]=1;m->to_w[i]=0;m->to_m[i]=1;i++;
    m->from_s[i]=0;m->from_r[i]=1;m->to_s[i]=0;m->to_w[i]=1;m->to_m[i]=1;i++;
    m->from_s[i]=1;m->from_r[i]=0;m->to_s[i]=0;m->to_w[i]=0;m->to_m[i]=-1;i++;
    m->from_s[i]=1;m->from_r[i]=1;m->to_s[i]=2;m->to_w[i]=1;m->to_m[i]=1;i++;
    m->from_s[i]=0;m->from_r[i]=2;m->to_s[i]=2;m->to_w[i]=2;m->to_m[i]=0;i++;
    m->from_s[i]=1;m->from_r[i]=2;m->to_s[i]=2;m->to_w[i]=2;m->to_m[i]=0;i++;
    return m;
}

static int stm_step(SpaceTM* m, int state, int read, int* next_s, int* write, int* move) {
    for(int i=0;i<m->n_trans;i++)
        if(m->from_s[i]==state && m->from_r[i]==read) { *next_s=m->to_s[i];*write=m->to_w[i];*move=m->to_m[i];return 1; }
    return 0;
}

/* Simulate TM for max_steps, return 0=reject, 1=accept, -1=timeout */
int space_tm_simulate(SpaceTM* m, const int* input, int len, int space_bound, int max_steps) {
    int tape[S_TAPE], head=S_TAPE/2;
    memset(tape,2,sizeof(tape));
    for(int i=0;i<len;i++) tape[head+i]=input[i];
    int state=m->q0, steps=0, max_head=head, min_head=head;
    while(steps<max_steps) {
        if(state==m->qa) return 1;
        if(state==m->qr) return 0;
        if(head<S_TAPE/2-space_bound || head>S_TAPE/2+space_bound) return 0;
        int nxt,wrt,mv;
        if(!stm_step(m,state,tape[head]<0?2:tape[head],&nxt,&wrt,&mv)) return 0;
        tape[head]=wrt; state=nxt; head+=mv; steps++;
        if(head>max_head) max_head=head;
        if(head<min_head) min_head=head;
        if(max_head-min_head > space_bound) return 0;
    }
    return -1;
}

void space_bounded_demo(void) {
    printf("\n===== Space-Bounded TM Simulator =====\n\n");
    SpaceTM* m = stm_create();
    int input[]={1,1,0,1}; int len=4;
    printf("Simulating TM on input [1,1,0,1] with space bounds:\n");
    printf("  space  result\n");
    for(int s=1;s<=6;s++) {
        int r = space_tm_simulate(m,input,len,s,200);
        printf("  %-6d %s\n", s, r==1?"ACCEPT":r==0?"REJECT":"TIMEOUT");
    }
    free(m->from_s);free(m->from_r);free(m->to_s);free(m->to_w);free(m->to_m);free(m);
    printf("\nA TM using s space has at most |Q|*s*|Gamma|^s configs.\n");
}