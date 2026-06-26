/* tm_enumerator.c — Enumerate all Turing Machines by Godel numbering
 * Every TM can be encoded as an integer (states, transitions).
 * This makes TMs countable: M_0, M_1, M_2, ...
 * The hierarchy uses: diagonalize against this enumeration. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int n_states; int n_syms; int** delta; int q0, qa, qr; } EncodedTM;

/* Decode Godel number into TM. Godel uses prime powers.
   Simplified: use index to select from a family. */
static EncodedTM* decode_tm(int index) {
    EncodedTM* m = malloc(sizeof(EncodedTM));
    m->n_states = 2 + (index % 4);
    m->n_syms = 2;
    m->q0 = 0; m->qa = 1; m->qr = m->n_states-1;
    m->delta = malloc((size_t)m->n_states * sizeof(int*));
    for(int q=0;q<m->n_states;q++) {
        m->delta[q] = malloc((size_t)m->n_syms * 3 * sizeof(int));
        for(int s=0;s<m->n_syms;s++) {
            int base = (index+q*7+s*13) % (m->n_states*2);
            m->delta[q][s*3+0] = base % 2;
            m->delta[q][s*3+1] = base % m->n_states;
            m->delta[q][s*3+2] = (base/3) % 3 - 1;
        }
    }
    return m;
}

static int tm_run(EncodedTM* m, const int* input, int len, int max_steps) {
    int tape[128], head=64;
    memset(tape,2,sizeof(tape));
    for(int i=0;i<len;i++) tape[head+i]=input[i];
    int state=m->q0;
    for(int t=0;t<max_steps;t++) {
        if(state==m->qa) return 1;
        if(state==m->qr) return 0;
        int sym=tape[head]>=m->n_syms?0:tape[head];
        int* d=&m->delta[state][sym*3];
        tape[head]=d[0]; state=d[1]; head+=d[2];
        if(head<0||head>=128) return 0;
    }
    return -1;
}

static void tm_free(EncodedTM*m){for(int q=0;q<m->n_states;q++)free(m->delta[q]);free(m->delta);free(m);}

void tm_enumerator_demo(void) {
    printf("\n===== TM Enumeration via Godel Numbering =====\n\n");
    printf("Every TM can be encoded as an integer.\n");
    printf("Therefore TMs are COUNTABLY infinite: M_0, M_1, M_2, ...\n\n");
    
    printf("Decoding TMs from indices:\n");
    printf("  index  states  syms\n");
    for(int i=0;i<5;i++) {
        EncodedTM* m = decode_tm(i);
        printf("  %-6d %-7d %-4d\n", i, m->n_states, m->n_syms);
        tm_free(m);
    }
    printf("\nThis enumeration is the basis of the time hierarchy proof:\n");
    printf("  Enumerate all poly-time TMs, then diagonalize.\n");
}