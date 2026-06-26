#ifndef PHASE_ANALYSIS_H
#define PHASE_ANALYSIS_H
#include "types.h"

CNF* generate_random_ksat(int n, int k, double ratio);
double benchmark_ratio(int n, int k, double ratio, int trials, int* sat_fraction);
void phase_transition_experiment(int n, int k, double ratio_min,
                                  double ratio_max, double ratio_step, int trials);
void phase_finite_size_scaling(int k, int n_min, int n_max, int step);
#endif
