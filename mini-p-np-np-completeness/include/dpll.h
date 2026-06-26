/* dpll.h — DPLL algorithm internals */
#ifndef DPLL_H
#define DPLL_H
#include "types.h"

int  dpll_solve(CNF* cnf, int* assign);
int  dpll_unit_propagate(CNF* cnf, int* assign);
int  dpll_pure_literal(CNF* cnf, int* assign);
int  dpll_choose_branch(const CNF* cnf, const int* assign, int n_vars);
void dpll_stats(void); /* solver statistics */

#endif
