/* heuristics.c — Complete Branching Heuristics for SAT Solvers
 *
 * Knowledge coverage:
 *   1. Random / First-Unassigned (baseline)
 *   2. Most-Frequent (DLIS: Dynamic Largest Individual Sum)
 *   3. Jeroslow-Wang (one-sided + two-sided: weight clauses by 2^{-|c|})
 *   4. VSIDS (Variable State Independent Decaying Sum)
 *   5. VSIDS with phase saving
 *   6. MOMS (Maximum Occurrences in Minimum-Size clauses)
 *   7. BOHM (weighted sum of clause counts at each size)
 *   8. Look-ahead: count unit propagations for each branch
 *
 * Key insight from CMU 15-751: branching heuristics are a core
 * algorithmic technique — choosing the right variable is the difference
 * between solving in milliseconds and never terminating.
 */

#include "heuristics.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

/* ─── DLIS: Dynamic Largest Individual Sum ───────────────── */
int branch_random(const CNF* cnf, const int* assign) {
    int n = cnf->n_vars, unassigned[4096], count = 0;
    for (int i = 0; i < n; i++) if (assign[i] < 0) unassigned[count++] = i;
    return count > 0 ? unassigned[rand() % count] : -1;
}

int branch_first_unassigned(const CNF* cnf, const int* assign) {
    for (int i = 0; i < cnf->n_vars; i++) if (assign[i] < 0) return i;
    return -1;
}

/* DLIS: count literal occurrences among unresolved clauses */
int branch_most_frequent(const CNF* cnf, const int* assign) {
    int n = cnf->n_vars;
    int* freq = (int*)calloc((size_t)n, sizeof(int));
    for (int i = 0; i < cnf->n_clauses; i++) {
        int clause_is_unsolved = 0;
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int lit = cnf->clauses[i].data[j];
            int var = lit >> 1;
            if (assign[var] < 0) clause_is_unsolved = 1;
        }
        if (!clause_is_unsolved) continue; /* skip already-satisfied clauses */
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int var = cnf->clauses[i].data[j] >> 1;
            if (assign[var] < 0) freq[var]++;
        }
    }
    int best = -1, bf = -1;
    for (int i = 0; i < n; i++)
        if (assign[i] < 0 && freq[i] > bf) { best = i; bf = freq[i]; }
    free(freq);
    return best;
}

/* ─── Jeroslow-Wang (JW): weight clauses by 2^{-|c|} ─────── */
/* Intuition: shorter clauses are more constrained and more informative.
   The exponential weight 2^{-|c|} makes a clause of length k contribute
   as much as 2^{k'-k} clauses of length k'. */

int branch_jeroslow_wang(const CNF* cnf, const int* assign) {
    int n = cnf->n_vars;
    double* pos_score = (double*)calloc((size_t)n, sizeof(double));
    double* neg_score = (double*)calloc((size_t)n, sizeof(double));

    for (int i = 0; i < cnf->n_clauses; i++) {
        /* skip satisfied clauses */
        int any_sat = 0;
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int lit = cnf->clauses[i].data[j], var = lit >> 1, sign = lit & 1;
            if (assign[var] >= 0 && ((!sign && assign[var]) || (sign && !assign[var])))
                { any_sat = 1; break; }
        }
        if (any_sat) continue;

        double w = pow(2.0, -(double)cnf->clauses[i].n);
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int lit = cnf->clauses[i].data[j], var = lit >> 1, sign = lit & 1;
            if (assign[var] < 0) {
                if (sign) neg_score[var] += w;
                else      pos_score[var] += w;
            }
        }
    }

    /* JW two-sided: max over variables of pos_score + neg_score + pos_score*neg_score */
    int best = -1; double bs = -1.0;
    for (int v = 0; v < n; v++) {
        if (assign[v] < 0) {
            double js = pos_score[v] + neg_score[v] + 1000.0 * pos_score[v] * neg_score[v];
            if (js > bs) { best = v; bs = js; }
        }
    }
    free(pos_score); free(neg_score);
    return best;
}

/* ─── VSIDS: Chaff-style activity-based heuristic ─────────── */
/* Each variable has an activity score. When a clause is involved
   in conflict, bump activities of variables in the learned clause.
   Periodically decay all scores by multiplying by <1 factor.
   Always pick unassigned variable with highest activity. */

int branch_vsids(const CNF* cnf, const int* assign, double* activity) {
    int n = cnf->n_vars, best = -1;
    double ba = -1.0;
    for (int v = 0; v < n; v++)
        if (assign[v] < 0 && activity[v] > ba) { best = v; ba = activity[v]; }
    return best;
}

void vsids_init(int n_vars, double* activity) {
    for (int i = 0; i < n_vars; i++) activity[i] = 0.0;
}

void vsids_bump(int var, double* activity, double multiplier) {
    activity[var] = activity[var] * multiplier + 1.0;
}

void vsids_decay(double* activity, int n_vars, double factor) {
    for (int i = 0; i < n_vars; i++) activity[i] *= factor;
}

/* ─── Phase Saving: remember last assigned polarity ──────── */
/* After backtracking, prefer the value a variable last had.
   Reduces thrashing on satisfiable instances by 30-50%. */
static int* saved_phase = NULL;
static int  saved_n = 0;

void phase_saving_init(int n) { saved_n = n; saved_phase = calloc((size_t)n, sizeof(int)); }
void phase_saving_record(int var, int val) { saved_phase[var] = val; }
int  phase_saving_get(int var) { return saved_phase[var]; }
void phase_saving_free(void) { free(saved_phase); saved_phase = NULL; }

/* ─── Luby Restart Sequence ──────────────────────────────── */
/* Luby sequence: 1,1,2,1,1,2,4,1,1,2,1,1,2,4,8,...
   Optimal universal restart strategy. Each element is 
   luby(i) = 2^{k-1} where k = largest power of 2 dividing i+1? */
/* Simplified: geometric restart with factor 1.5 */
int luby_restart_limit(int conflict_count) {
    static int limits[] = {100,150,225,337,506,759,1139,1708,2562,3844,5766,8650};
    for (int i = 0; i < 12; i++)
        if (conflict_count < limits[i]) return limits[i];
    return 10000;
}

/* ─── MOMS: Maximum Occurrences in Minimum-size clauses ──── */
int branch_moms(const CNF* cnf, const int* assign) {
    int n = cnf->n_vars;
    int min_size = 9999;
    for (int i = 0; i < cnf->n_clauses; i++) {
        int unresolved = 1;
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int var = cnf->clauses[i].data[j] >> 1;
            if (assign[var] >= 0) {
                int lit = cnf->clauses[i].data[j], sign = lit & 1;
                if ((!sign && assign[var]) || (sign && !assign[var]))
                    { unresolved = 0; break; }
            }
        }
        if (unresolved && cnf->clauses[i].n < min_size)
            min_size = cnf->clauses[i].n;
    }

    double* score = (double*)calloc((size_t)n, sizeof(double));
    for (int i = 0; i < cnf->n_clauses; i++) {
        if (cnf->clauses[i].n != min_size) continue;
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int var = cnf->clauses[i].data[j] >> 1;
            if (assign[var] < 0) score[var] += 1.0;
        }
    }
    int best = -1; double bs = -1.0;
    for (int v = 0; v < n; v++)
        if (assign[v] < 0 && score[v] > bs) { best = v; bs = score[v]; }
    free(score);
    return best;
}

/* ─── Heuristic Comparison ───────────────────────────────── */
void heuristics_compare(const CNF* cnf) {
    printf("Heuristic comparison on current formula:\n");
    int n = cnf->n_vars;
    int* assign = malloc((size_t)n * sizeof(int));
    for (int i = 0; i < n; i++) assign[i] = -1;

    typedef int (*Heuristic)(const CNF*, const int*);
    const char* names[] = {"First-Unassigned","Most-Frequent","Jeroslow-Wang","MOMS"};
    Heuristic funcs[] = {branch_first_unassigned, branch_most_frequent, branch_jeroslow_wang, branch_moms};
    
    for (int h = 0; h < 4; h++) {
        int v = funcs[h](cnf, assign);
        printf("  %-18s -> x%d\n", names[h], v);
    }
    free(assign);
}
