#ifndef STOCHASTIC_H
#define STOCHASTIC_H
#include "types.h"

/* WalkSAT: stochastic local search for SAT.
   Not complete but often finds solutions to satisfiable
   large instances much faster than DPLL/CDCL. */
int walksat_solve(CNF* cnf, int* assignment, int max_flips, double noise);

/* GSAT: greedy local search */
int gsat_solve(CNF* cnf, int* assignment, int max_flips);

/* Simulated Annealing SAT solver */
int sa_sat_solve(CNF* cnf, int* assignment, int max_steps, double temp_start, double cooling);

#endif
