/*
 * cook_levin.c — Full Cook-Levin Reduction & Verification
 *
 * L4 Fundamental Law:
 *   SAT is NP-complete. For any language L in NP decided by NTM M
 *   in time p(n), there exists a polynomial-time reduction f such that
 *   for all w: w in L <=> f(w) in SAT.
 *
 * This module:
 *   1. Implements the full reduction (NTM + input -> CNF)
 *   2. Generates all four clause groups
 *   3. Verifies the correctness of the reduction
 *   4. Extracts computation paths from SAT assignments
 *   5. Provides polynomial bound analysis
 *
 * Reference: Cook (1971), Levin (1973), Sipser section 7.4, Arora-Barak section 2.3
 */

#include "cook_levin.h"
#include "turing_machine.h"
#include "sat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* ================================================================
 * Full Cook-Levin Reduction
 * ================================================================ */

CNF* cook_levin_reduce(const TuringMachine* tm,
                        const Symbol* input, int input_len, int T) {
    if (!tm || !input || T <= 0) return NULL;

    /* Compute space bound: P = T + input_len + 2 (sufficient for any computation) */
    int P = T + input_len + 2;

    /* Determine blank symbol index — assume 0 if tape alphabet is set, else -1 */
    int blank_idx = 0;  /* Default: index 0 in tape alphabet = BLANK_SYMBOL */

    /* Create variable scheme */
    int n_symbols = tm->tape_alpha_size > 0 ? tm->tape_alpha_size : 4;
    CLVariableScheme vs = cl_make_var_scheme(T, P, tm->n_states, n_symbols);

    /* Create CNF with estimated capacity */
    long long est_clauses = (long long)(T + 1) * P * n_symbols * n_symbols / 2  /* cell consistency */
                          + (long long)(T + 1) * P * (P - 1) / 2               /* head uniqueness */
                          + (long long)(T + 1) * tm->n_states * (tm->n_states - 1) / 2 /* state uniq */
                          + (long long)T * P * tm->num_trans * 4               /* transitions */
                          + T * P * n_symbols + 1;                            /* inertia + acceptance */

    int clause_cap = est_clauses > 100000 ? 100000 : (int)est_clauses + 10;

    CNF* cnf = cnf_create(vs.total_vars, clause_cap);

    /* G1: Cell consistency */
    cl_add_cell_consistency(cnf, &vs);

    /* G2: Initial configuration + uniqueness */
    cl_add_initial_config(cnf, &vs, input, input_len, tm->q0, blank_idx);
    cl_add_head_uniqueness(cnf, &vs);
    cl_add_state_uniqueness(cnf, &vs);

    /* G3: Transition validity */
    cl_add_transition_clauses(cnf, &vs, tm, blank_idx);

    /* G4: Acceptance */
    cl_add_acceptance(cnf, &vs, tm->q_accept);

    return cnf;
}

/* ================================================================
 * Detailed Reduction with Statistics
 * ================================================================ */

CLReduction* cook_levin_reduce_detailed(const TuringMachine* tm,
                                         const Symbol* input, int input_len,
                                         int T) {
    CLReduction* red = (CLReduction*)calloc(1, sizeof(CLReduction));
    if (!red) return NULL;

    clock_t t0 = clock();

    int P = T + input_len + 2;
    int n_symbols = tm->tape_alpha_size > 0 ? tm->tape_alpha_size : 4;
    CLVariableScheme vs = cl_make_var_scheme(T, P, tm->n_states, n_symbols);

    long long est = (long long)(T + 1) * P * n_symbols * n_symbols
                  + (long long)T * P * tm->num_trans * 5 + 1000;
    int cap = est > 1000000 ? 1000000 : (int)est + 100;

    CNF* cnf = cnf_create(vs.total_vars, cap);
    int n_before;

    /* G1 */
    n_before = cnf->n_clauses;
    cl_add_cell_consistency(cnf, &vs);
    red->n_g1 = cnf->n_clauses - n_before;

    /* G2 */
    n_before = cnf->n_clauses;
    cl_add_initial_config(cnf, &vs, input, input_len, tm->q0, 0);
    cl_add_head_uniqueness(cnf, &vs);
    cl_add_state_uniqueness(cnf, &vs);
    red->n_g2 = cnf->n_clauses - n_before;

    /* G3 */
    n_before = cnf->n_clauses;
    cl_add_transition_clauses(cnf, &vs, tm, 0);
    red->n_g3 = cnf->n_clauses - n_before;

    /* G4 */
    n_before = cnf->n_clauses;
    cl_add_acceptance(cnf, &vs, tm->q_accept);
    red->n_g4 = cnf->n_clauses - n_before;

    clock_t t1 = clock();
    red->formula = cnf;
    red->n_vars = vs.total_vars;
    red->n_clauses = cnf->n_clauses;
    red->gen_time_ms = (double)(t1 - t0) / CLOCKS_PER_SEC * 1000.0;

    return red;
}

void cl_reduction_free(CLReduction* red) {
    if (!red) return;
    cnf_free(red->formula);
    free(red);
}

/* ================================================================
 * Verification of Cook-Levin Correctness
 * ================================================================ */

int cook_levin_verify(const TuringMachine* tm,
                      const Symbol* input, int input_len, int T) {
    if (!tm || !input) return 0;

    /* Direction 1: If M accepts, phi should be SAT */
    CNF* phi = cook_levin_reduce(tm, input, input_len, T);
    if (!phi) return 0;

    int ntm_result = tm_simulate_nondeterministic(tm, input, input_len, T, 100000);

    Assignment assign = assignment_create(phi->n_vars);
    int sat_result = sat_solve_dpll(phi, assign);

    int dir1_ok = (ntm_result == 1) ? (sat_result == 1) : 1;
    int dir2_ok = (sat_result == 1) ? sat_verify(phi, assign) : 1;

    /* If SAT, verify the extracted path */
    if (sat_result == 1 && ntm_result == 1) {
        int P = T + input_len + 2;
        int n_symbols = tm->tape_alpha_size > 0 ? tm->tape_alpha_size : 4;
        CLVariableScheme vs = cl_make_var_scheme(T, P, tm->n_states, n_symbols);

        TMComputationPath* path = cook_levin_extract_path(&vs, assign, tm,
                                                           input, input_len, T);
        if (path) {
            tm_path_free(path);
        } else {
            dir2_ok = 0;
        }
    }

    assignment_free(assign);
    cnf_free(phi);

    return dir1_ok && dir2_ok;
}

/* ================================================================
 * Extract Computation Path from SAT Assignment
 * ================================================================ */

TMComputationPath* cook_levin_extract_path(const CLVariableScheme* vs,
                                            const Assignment assign,
                                            const TuringMachine* tm,
                                            const Symbol* input,
                                            int input_len, int T) {
    if (!vs || !assign || !tm) return NULL;
    (void)input; (void)input_len;  /* reserved for future path validation */

    TMComputationPath* path = tm_path_create();

    for (int t = 0; t <= T; t++) {
        /* Find state at time t */
        int state = -1;
        for (int q = 0; q < tm->n_states; q++) {
            int v = cl_var_state(vs, t, q);
            if (v >= 0 && v < vs->total_vars && assign[v] == 1) {
                state = q;
                break;
            }
        }
        if (state < 0) {
            tm_path_free(path);
            return NULL;
        }

        TMConfiguration* cfg = tm_config_create(state);

        /* Find head position at time t */
        int head_p = -1;
        for (int p = 0; p < vs->P; p++) {
            int v = cl_var_head(vs, t, p);
            if (v >= 0 && v < vs->total_vars && assign[v] == 1) {
                head_p = p;
                break;
            }
        }
        if (head_p >= 0) cfg->head_pos = head_p;

        /* Extract tape contents */
        for (int p = 0; p < vs->P; p++) {
            for (int s = 0; s < vs->n_symbols; s++) {
                int v = cl_var_cell(vs, t, p, s);
                if (v >= 0 && v < vs->total_vars && assign[v] == 1) {
                    /* Found symbol s at position p */
                    while (p >= cfg->tape_len) {
                        cfg->tape_cap *= 2;
                        cfg->tape = (Symbol*)realloc(cfg->tape,
                                      (size_t)cfg->tape_cap * sizeof(Symbol));
                        for (int i = cfg->tape_len; i < cfg->tape_cap; i++)
                            cfg->tape[i] = BLANK_SYMBOL;
                        cfg->tape_len = cfg->tape_cap;
                    }
                    cfg->tape[p] = (Symbol)s;  /* symbol index = s */
                    break;
                }
            }
        }

        tm_path_add_config(path, cfg);
        tm_config_free(cfg);

        /* Check if we reached accept state */
        if (state == tm->q_accept) break;
    }

    return path;
}

/* ================================================================
 * Polynomial Bound Analysis
 * ================================================================ */

PolyBound cl_compute_poly_bound(int input_len, int degree, double coeff) {
    PolyBound pb;
    pb.degree = degree;
    pb.coefficient = coeff;
    /* bound = ceil(coeff * n^degree) */
    double val = coeff;
    for (int i = 0; i < degree; i++) val *= (double)input_len;
    pb.bound = (int)(val + 0.999999);
    if (pb.bound < 1) pb.bound = 1;
    return pb;
}

CLSizeEstimate cl_estimate_size(int T, int P, int n_states, int n_symbols) {
    CLSizeEstimate est;
    est.est_vars = (long long)(T + 1) * P * n_symbols
                 + (long long)(T + 1) * P
                 + (long long)(T + 1) * n_states;

    est.est_clauses = (long long)(T + 1) * P * n_symbols * n_symbols / 2   /* G1 */
                    + (long long)(T + 1) * P * (P - 1) / 2                 /* G2 head */
                    + (long long)(T + 1) * n_states * (n_states - 1) / 2   /* G2 state */
                    + (long long)T * P * n_symbols * 20                    /* G3 */
                    + 1000;                                                 /* overhead */

    est.est_memory_bytes = est.est_clauses * 512;  /* rough estimate */
    return est;
}