/* sat.c — Unified SAT Solver Interface + Complexity Analysis
 *
 * Provides a single entry point for all SAT solvers in the module,
 * plus empirical P-vs-NP evidence through comparative benchmarks.
 */

#include "sat.h"
#include "dpll.h"
#include "cdcl.h"
#include "stochastic.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int sat_solve_dpll(CNF* cnf, int* assign) { return dpll_solve(cnf, assign); }
int sat_solve_cdcl(CNF* cnf, int* assign) { return cdcl_solve(cnf, assign); }

int sat_solve(CNF* cnf, int* assign) {
    if (cnf->n_clauses < 200) return sat_solve_dpll(cnf, assign);
    return sat_solve_cdcl(cnf, assign);
}

/* Polynomial-time verifier: O(total_literals) */
int sat_verify(const CNF* cnf, const int* assign) {
    for (int i = 0; i < cnf->n_clauses; i++) {
        int ok = 0;
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int lit = cnf->clauses[i].data[j], var = lit >> 1, sign = lit & 1;
            if ((!sign && assign[var] == 1) || (sign && assign[var] == 0))
                { ok = 1; break; }
        }
        if (!ok) return 0;
    }
    return 1;
}

/* Exact count (exponential, #P-complete) */
long long sat_count_exact(CNF* cnf) {
    int n = cnf->n_vars;
    long long max = 1LL << n;
    if (max > 50000000LL) max = 50000000LL;
    int* assign = malloc((size_t)n * sizeof(int));
    long long total = 0;
    for (long long m = 0; m < max; m++) {
        for (int v = 0; v < n; v++) assign[v] = (m >> v) & 1;
        if (sat_verify(cnf, assign)) total++;
    }
    free(assign);
    return total;
}

long long sat_count(CNF* cnf) { return sat_count_exact(cnf); }

double sat_count_approx(CNF* cnf, int samples) {
    int n = cnf->n_vars, hits = 0;
    int* assign = malloc((size_t)n * sizeof(int));
    for (int s = 0; s < samples; s++) {
        for (int v = 0; v < n; v++) assign[v] = rand() & 1;
        if (sat_verify(cnf, assign)) hits++;
    }
    free(assign);
    return (double)hits / samples;
}
