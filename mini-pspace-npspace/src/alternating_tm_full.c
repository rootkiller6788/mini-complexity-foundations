/* alternating_tm_full.c — Alternating Turing Machine simulation
 * Chandra-Kozen-Stockmeyer (1981): ATIME(poly) = PSPACE
 * An ATM has existential AND universal states.
 * EXIST state: accept if ANY transition leads to acceptance.
 * FORALL state: accept if ALL transitions lead to acceptance. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ATM_TAPE 64
#define ATM_MAX_TRANS 8

typedef enum { Q_EXIST, Q_FORALL, Q_ACCEPT, Q_REJECT } ATMStateType;
typedef struct { int from; int read; int to; int write; int move; } ATMTrans;

typedef struct { int n_states; ATMStateType* types; int n_trans; ATMTrans trans[ATM_MAX_TRANS]; int q0; } ATM;

static ATM* atm_create_simple(void) {
    ATM* m = malloc(sizeof(ATM));
    m->n_states = 4; m->q0 = 0;
    m->types = malloc(4*sizeof(ATMStateType));
    m->types[0]=Q_EXIST; m->types[1]=Q_FORALL; m->types[2]=Q_ACCEPT; m->types[3]=Q_REJECT;
    m->n_trans = 0;
    m->trans[m->n_trans++]=(ATMTrans){0,1,2,1,1};
    m->trans[m->n_trans++]=(ATMTrans){0,0,1,0,1};
    m->trans[m->n_trans++]=(ATMTrans){1,1,2,1,1}; /* forall: accept if 1 */
    m->trans[m->n_trans++]=(ATMTrans){1,0,3,0,0}; /* forall: reject if 0 */
    return m;
}

static int atm_eval(ATM* m, int state, int* tape, int head, int steps, int max_steps) {
    if(steps > max_steps) return 0;
    if(m->types[state] == Q_ACCEPT) return 1;
    if(m->types[state] == Q_REJECT) return 0;
    
    int sym = tape[head];
    int results[8], nr=0;
    
    for(int i=0;i<m->n_trans;i++) {
        if(m->trans[i].from == state && m->trans[i].read == sym) {
            int new_head = head + m->trans[i].move;
            if(new_head<0||new_head>=ATM_TAPE) continue;
            int saved = tape[head];
            tape[head] = m->trans[i].write;
            int r = atm_eval(m, m->trans[i].to, tape, new_head, steps+1, max_steps);
            tape[head] = saved;
            results[nr++] = r;
        }
    }
    if(nr == 0) return 0;
    
    if(m->types[state] == Q_EXIST) {
        for(int i=0;i<nr;i++) if(results[i]) return 1;
        return 0;
    } else { /* FORALL */
        for(int i=0;i<nr;i++) if(!results[i]) return 0;
        return 1;
    }
}

void alternating_tm_demo(void) {
    printf("\n===== Alternating Turing Machine =====\n\n");
    printf("Chandra-Kozen-Stockmeyer (1981): ATIME(poly) = PSPACE\n");
    printf("ATM has: EXIST states (any branch leads to accept)\n");
    printf("        FORALL states (all branches lead to accept)\n\n");
    
    ATM* m = atm_create_simple();
    int tape[ATM_TAPE]={1,0};
    int r = atm_eval(m, 0, tape, ATM_TAPE/2, 0, 10);
    printf("ATM on input [1,0]: %s\n", r?"ACCEPT":"REJECT");
    free(m->types); free(m);
    
    printf("\nATIME(f) = languages decided by ATM in O(f) time.\n");
    printf("AP = ATIME(poly) = PSPACE.\n");
    printf("ALOGSPACE = P, ASPACE(log n) = P.\n");
}