/* cdcl.h — Conflict-Driven Clause Learning solver */
#ifndef CDCL_H
#define CDCL_H
#include "types.h"

int cdcl_solve(CNF* cnf, int* assign);

/* learning and heuristics */
void cdcl_learn_clause(CNF* cnf, const int* assign, int conflict_clause);
int  cdcl_bcp(CNF* cnf, int* assign);           /* boolean constraint propagation */
int  cdcl_analyze_conflict(CNF* cnf, int* assign, int* learnt);
int  cdcl_restart(void);                        /* restart heuristic */
int  cdcl_decay_activity(CNF* cnf);             /* VSIDS scores */

typedef struct { int var; double activity; } VSIDS;

void cdcl_stats(void);

#endif
