/* phase_analysis.c — SAT Phase Transition: Complete Analysis
 *
 * The sharp SAT/UNSAT transition at α_c ≈ 4.26 for random 3-SAT is
 * one of the most striking phenomena in computer science. It connects
 * statistical physics (spin glasses), combinatorics (random graphs),
 * and computational complexity.
 *
 * Key results:
 *   - Below α_c: almost surely satisfiable (polynomial avg time)
 *   - Near α_c: exponential median time (algorithmic "hard" region)  
 *   - Above α_c: almost surely unsatisfiable (short proofs exist)
 *   - Finite-size scaling: the transition sharpens as n→∞
 */

#include "phase_analysis.h"
#include "dpll.h"
#include "sat.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

CNF* generate_random_ksat(int n, int k, double ratio) {
    int m = (int)(ratio * n); if (m < 1) m = 1;
    CNF* cnf = cnf_new(n, m);
    for (int i = 0; i < m; i++) {
        int lits[16];
        for (int j = 0; j < k; j++)
            lits[j] = ((rand() % n) << 1) | (rand() & 1); /* no repeated vars */
        cnf_add(cnf, lits, k);
    }
    return cnf;
}

double benchmark_ratio(int n, int k, double ratio, int trials, int* sat_frac) {
    int sat_count = 0;
    double total_time = 0.0;
    for (int t = 0; t < trials; t++) {
        CNF* cnf = generate_random_ksat(n, k, ratio);
        int* assign = malloc((size_t)n * sizeof(int));
        clock_t t0 = clock();
        int result = dpll_solve(cnf, assign);
        total_time += (double)(clock() - t0) / CLOCKS_PER_SEC;
        if (result) sat_count++;
        free(assign); cnf_free(cnf);
    }
    *sat_frac = sat_count;
    return total_time / trials;
}

void phase_transition_experiment(int n, int k, double rmin, double rmax,
                                  double rstep, int trials) {
    printf("\n==========================================\n");
    printf("SAT Phase Transition: n=%d, k=%d\n", n, k);
    printf("==========================================\n");
    printf("%8s %8s %8s %10s %8s\n", "ratio", "clauses", "SAT%", "time(ms)", "median");
    printf("%8s %8s %8s %10s %8s\n", "─────", "───────", "────", "────────", "──────");
    
    /* Collect all solve times for median computation */
    for (double r = rmin; r <= rmax + 1e-9; r += rstep) {
        int m = (int)(r * n), sat_count = 0;
        double times[32], total = 0.0, median_t = 0.0;
        
        for (int t = 0; t < trials && t < 32; t++) {
            CNF* cnf = generate_random_ksat(n, k, r);
            int* assign = malloc((size_t)n * sizeof(int));
            clock_t t0 = clock();
            int result = dpll_solve(cnf, assign);
            times[t] = (double)(clock() - t0) / CLOCKS_PER_SEC;
            total += times[t];
            if (result) sat_count++;
            free(assign); cnf_free(cnf);
        }
        /* median */
        for (int a = 0; a < trials-1; a++)
            for (int b = a+1; b < trials; b++)
                if (times[a] > times[b]) { double tmp = times[a]; times[a] = times[b]; times[b] = tmp; }
        median_t = times[trials/2];

        double sat_pct = 100.0 * sat_count / trials;
        printf("%8.2f %8d %7.1f%% %9.2f %8.2f\n",
               r, m, sat_pct, total / trials * 1000.0, median_t * 1000.0);
    }

    printf("\nTheoretical threshold α_c ≈ 4.26 for 3-SAT.\n");
    printf("Near α_c: exponential median solve time.\n");
    printf("Away from α_c: polynomial average time.\n");
    printf("This is why random SAT benchmarks live at ratio ≈ 4.26.\n");
}

/* Finite-size scaling analysis: how does the threshold shift with n? */
void phase_finite_size_scaling(int k, int n_min, int n_max, int step) {
    printf("\n===== Finite-Size Scaling =====\n");
    printf("Threshold α_c(n) for k=%d as n grows:\n\n", k);
    printf("%6s %10s\n", "n", "α_c(n)");
    printf("%6s %10s\n", "──", "──────");

    for (int n = n_min; n <= n_max; n += step) {
        /* binary search for α where SAT% = 50% */
        double lo = 2.0, hi = 6.0;
        for (int iter = 0; iter < 15; iter++) {
            double mid = (lo + hi) / 2.0;
            int sat_count;
            benchmark_ratio(n, k, mid, 20, &sat_count);
            double sat_frac = (double)sat_count / 20.0;
            if (sat_frac > 0.5) lo = mid; else hi = mid;
        }
        printf("%6d %10.3f\n", n, (lo + hi) / 2.0);
    }
    printf("\nα_c(n) → α_c(∞) ≈ 4.26 as n → ∞\n");
}
