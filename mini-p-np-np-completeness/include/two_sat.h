/* two_sat.h — 2-SAT solver (polynomial via SCC/implication graph) */
#ifndef TWO_SAT_H
#define TWO_SAT_H

/* 2-SAT: each clause has exactly 2 literals. Solved in O(V+E)
   by building implication graph and checking SCCs.
   Demonstrates that 2-SAT ∈ P (vs 3-SAT ∈ NP-C). */
int two_sat_solve(int n_vars, int n_clauses, int clauses[][2], int* assignment);

#endif
