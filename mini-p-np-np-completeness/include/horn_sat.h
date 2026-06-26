/* horn_sat.h — Horn SAT solver (polynomial time, P-complete) */
#ifndef HORN_SAT_H
#define HORN_SAT_H
#include "types.h"

/* A Horn clause has at most one positive literal.
   Horn-SAT is in P (linear time). This demonstrates
   that NOT all SAT variants are NP-complete. */
int horn_sat_solve(CNF* cnf, int* assignment);
int horn_sat_is_horn(const CNF* cnf);

#endif
