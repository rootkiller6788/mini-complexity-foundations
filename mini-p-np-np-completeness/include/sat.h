/* sat.h — SAT solver core API */
#ifndef SAT_H
#define SAT_H
#include "types.h"

/* DPLL: classic backtracking + unit propagation + pure literal */
int sat_solve_dpll(CNF* cnf, int* assignment);

/* CDCL: conflict-driven clause learning (modern solver) */
int sat_solve_cdcl(CNF* cnf, int* assignment);

/* default solver (auto-selects best algorithm) */
int sat_solve(CNF* cnf, int* assignment);

/* polynomial-time verifier */
int sat_verify(const CNF* cnf, const int* assignment);

/* #SAT: exact count of satisfying assignments */
long long sat_count_exact(CNF* cnf);
long long sat_count(CNF* cnf);  /* alias */

/* approximate count via sampling */
double sat_count_approx(CNF* cnf, int samples);

#endif
