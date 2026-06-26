#ifndef HEURISTICS_H
#define HEURISTICS_H
#include "types.h"

int branch_random(const CNF* c, const int* a);
int branch_first_unassigned(const CNF* c, const int* a);
int branch_most_frequent(const CNF* c, const int* a);
int branch_jeroslow_wang(const CNF* c, const int* a);
int branch_vsids(const CNF* c, const int* a, double* activity);
int branch_moms(const CNF* c, const int* a);

void vsids_init(int n, double* act);
void vsids_bump(int var, double* act, double mult);
void vsids_decay(double* act, int n, double factor);

void phase_saving_init(int n);
void phase_saving_record(int var, int val);
int  phase_saving_get(int var);
void phase_saving_free(void);

int luby_restart_limit(int conflicts);
void heuristics_compare(const CNF* c);
#endif
