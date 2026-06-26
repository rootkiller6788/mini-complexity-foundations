/* resolution.h — Resolution proof system */
#ifndef RESOLUTION_H
#define RESOLUTION_H
#include "types.h"

/* Resolution rule: from (A ∨ x) and (B ∨ ¬x) derive (A ∨ B) */
int resolution_apply(const int* cl_a, int na, const int* cl_b, int nb,
                     int* resolvent, int max_n);

/* Check if empty clause (contradiction) is derivable via resolution.
   Resolution is sound and refutation-complete. */
int resolution_refute(CNF* cnf, int max_steps);

/* DAG-based resolution proof search with subsumption checking.
   Returns 1 if empty clause derived, stores step count. */
int resolution_refute_full(CNF* cnf, int max_steps, int* proof_steps);

/* Exhaustive unit propagation for resolution (complete for Horn). */
int resolution_unit_propagate(CNF* cnf, int* assign);

/* Clause A subsumes clause B if every literal of A appears in B. */
int resolution_subsumes(const int* a, int na, const int* b, int nb);

/* Build Pigeonhole Principle formula PHP^{n+1}_n (exponential lower bound). */
CNF* resolution_pigeonhole(int n);

/* Print resolution proof DAG. */
void print_proof_dag(int target);

/* Resolution proof tree (for visualization) */
typedef struct ResNode {
    int*   clause; int n;
    struct ResNode *parent_a, *parent_b;
    int    pivot_var;
} ResNode;
ResNode* resolution_prove(CNF* cnf);

#endif
