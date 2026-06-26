/* stochastic.c — WalkSAT, GSAT, Simulated Annealing SAT solvers */
#include "stochastic.h"
#include "sat.h"  /* for sat_verify */
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* ─── Utility: count unsatisfied clauses ──────────────────── */
static int count_unsat(const CNF* cnf, const int* assign) {
    int unsat = 0;
    for (int i = 0; i < cnf->n_clauses; i++) {
        int sat = 0;
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int lit = cnf->clauses[i].data[j];
            int var = lit >> 1, sign = lit & 1;
            if ((!sign && assign[var] == 1) || (sign && assign[var] == 0)) {
                sat = 1; break;
            }
        }
        if (!sat) unsat++;
    }
    return unsat;
}

/* ─── Pick random unsatisfied clause ──────────────────────── */
static int pick_unsat_clause(const CNF* cnf, const int* assign) {
    int unsat[4096], count = 0;
    for (int i = 0; i < cnf->n_clauses && count < 4096; i++) {
        int sat = 0;
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int lit = cnf->clauses[i].data[j];
            int var = lit >> 1, sign = lit & 1;
            if ((!sign && assign[var] == 1) || (sign && assign[var] == 0)) {
                sat = 1; break;
            }
        }
        if (!sat) unsat[count++] = i;
    }
    return count > 0 ? unsat[rand() % count] : -1;
}

/* ─── WalkSAT ─────────────────────────────────────────────── */
int walksat_solve(CNF* cnf, int* assign, int max_flips, double noise) {
    int n = cnf->n_vars;
    /* Random initial assignment */
    for (int i = 0; i < n; i++) assign[i] = rand() & 1;

    for (int flip = 0; flip < max_flips; flip++) {
        if (sat_verify(cnf, assign)) return 1;

        int ci = pick_unsat_clause(cnf, assign);
        if (ci < 0) continue;

        const Clause* c = &cnf->clauses[ci];

        /* With probability noise: pick random literal from clause */
        if ((double)rand() / RAND_MAX < noise) {
            int li = rand() % c->n;
            int var = c->data[li] >> 1;
            assign[var] ^= 1; /* flip */
        } else {
            /* Greedy: pick literal that minimizes new unsat count */
            int best_lit = -1, best_unsat = INT_MAX;
            for (int j = 0; j < c->n; j++) {
                int var = c->data[j] >> 1;
                int saved = assign[var];
                assign[var] ^= 1;
                int unsat = count_unsat(cnf, assign);
                assign[var] = saved;
                if (unsat < best_unsat) {
                    best_unsat = unsat; best_lit = j;
                }
            }
            if (best_lit >= 0) {
                int var = c->data[best_lit] >> 1;
                assign[var] ^= 1;
            }
        }
    }
    return 0; /* not found within max_flips */
}

/* ─── GSAT ────────────────────────────────────────────────── */
int gsat_solve(CNF* cnf, int* assign, int max_flips) {
    int n = cnf->n_vars;
    for (int i = 0; i < n; i++) assign[i] = rand() & 1;

    for (int flip = 0; flip < max_flips; flip++) {
        if (sat_verify(cnf, assign)) return 1;

        /* Pick variable whose flip minimizes unsat clauses */
        int best_var = -1, best_unsat = INT_MAX;
        for (int v = 0; v < n; v++) {
            assign[v] ^= 1;
            int unsat = count_unsat(cnf, assign);
            assign[v] ^= 1;
            if (unsat < best_unsat) { best_unsat = unsat; best_var = v; }
        }
        if (best_var >= 0) assign[best_var] ^= 1;
    }
    return 0;
}

/* ─── Simulated Annealing SAT ─────────────────────────────── */
int sa_sat_solve(CNF* cnf, int* assign, int max_steps, double T0, double cooling) {
    int n = cnf->n_vars;
    for (int i = 0; i < n; i++) assign[i] = rand() & 1;
    int current_unsat = count_unsat(cnf, assign);
    double T = T0;

    for (int step = 0; step < max_steps; step++) {
        if (current_unsat == 0) return 1;

        /* Random flip */
        int var = rand() % n;
        int saved = assign[var];
        assign[var] ^= 1;
        int new_unsat = count_unsat(cnf, assign);

        int delta = new_unsat - current_unsat;
        if (delta <= 0) {
            current_unsat = new_unsat; /* accept improvement */
        } else {
            double p = exp(-delta / T);
            if ((double)rand() / RAND_MAX < p) {
                current_unsat = new_unsat; /* accept with probability */
            } else {
                assign[var] = saved; /* reject */
            }
        }
        T *= cooling;
    }
    return 0;
}
