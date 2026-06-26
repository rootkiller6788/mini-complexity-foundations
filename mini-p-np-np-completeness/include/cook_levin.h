/* cook_levin.h — Cook-Levin theorem: NTM tableau → CNF reduction */
#ifndef COOK_LEVIN_H
#define COOK_LEVIN_H
#include "types.h"

/* A nondeterministic Turing machine for the Cook-Levin demo.
   Limited to: Q={0,1,2=accept,3=reject}, Σ={0,1}, Γ={0,1,□}
   Transitions encoded compactly. */

typedef struct {
    int n_states;
    int q0, q_accept, q_reject;
    /* delta[state][read] -> list of (next_state, write, dir) */
    int n_trans;
    int* trans_state;
    int* trans_read;
    int* trans_next;
    int* trans_write;
    int* trans_dir;   /* -1=L, 0=S, +1=R */
} NTM_cook;

NTM_cook* ntm_create_simple(void);
void ntm_free(NTM_cook* m);
void ntm_add_delta(NTM_cook* m, int q, int sym, int q2, int w, int d);

/* Simulate NTM nondeterministically; max T steps */
int ntm_simulate(const NTM_cook* m, const int* input, int len, int T);

/* Cook-Levin reduction:
   Given NTM M and input w, produce CNF φ s.t.
   M accepts w in ≤ T steps  ⇔  φ is satisfiable.
   
   T = time bound (must be polynomial)
   Returns: CNF* that is SAT iff M accepts w within T steps */
CNF* cook_levin_tableau(const NTM_cook* m, const int* input, int input_len, int T);

#endif
