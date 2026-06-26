/* ph_oracle.c - Oracle Machine Simulation for Polynomial Hierarchy
 *
 * The oracle definition of PH:
 *   Sigma_0 = P
 *   Sigma_{k+1} = NP^{Sigma_k}
 *   Pi_k = co-Sigma_k
 *
 * This file implements:
 *   L2-L3: Oracle Turing machine simulation
 *   L4: Oracle hierarchy equivalence to quantifier hierarchy
 *   L5: Oracle query resolution via SAT/DPLL
 *
 * Key insight: An NP^{C} machine can decide languages by:
 *   1. Nondeterministically guessing a witness w
 *   2. Making poly-many queries to oracle C about w
 *   3. Accepting iff all oracle answers are favorable
 *
 * For Sigma_2 = NP^{SAT}: guess x, ask SAT if forall y holds.
 */
#include "ph.h"
#include <string.h>

/* ================================================================
 * Oracle Machine Infrastructure
 * ================================================================ */

PH_OracleMachine* ph_oracle_create(int input, int witness_len, int query_len) {
    PH_OracleMachine* m = malloc(sizeof(PH_OracleMachine));
    if (!m) return NULL;
    memset(m, 0, sizeof(PH_OracleMachine));
    m->input = input;
    m->witness_len = witness_len;
    m->query_len = query_len;
    m->n_queries = 0;
    m->query_results = malloc((size_t)witness_len * sizeof(int));
    if (!m->query_results) { free(m); return NULL; }
    memset(m->query_results, 0, (size_t)witness_len * sizeof(int));
    return m;
}

void ph_oracle_free(PH_OracleMachine* m) {
    if (m) { free(m->query_results); free(m); }
}

/* Encode an oracle query as an integer from a CNF.
 * Simple encoding: hash the CNF structure into a query integer. */
static int encode_query(const PH_CNF* f, int witness) {
    /* A simple query encoding: combine formula hash with witness.
     * In reality, queries would be SAT instances.
     * We use a simple hash for demonstration. */
    unsigned int h = (unsigned int)witness * 2654435761U;
    for (int i = 0; i < f->n_clauses && i < 16; i++) {
        for (int j = 0; j < 3; j++) {
            h = h * 31 + (unsigned int)(f->clauses[i].lits[j] + 1);
        }
    }
    return (int)(h & 0x7FFFFFFF);
}

/* ================================================================
 * Sigma_1 Oracle: NP = NP^P
 *
 * Since P oracle is useless (can be simulated in poly time),
 * NP^P = NP. This is a baseline oracle machine.
 * ================================================================ */

int ph_sigma1_oracle_solve(const PH_CNF* f) {
    /* NP^P = NP: use DPLL SAT as the "oracle" (which is in P... actually
     * SAT is NP-complete, but DPLL is an exponential algorithm.
     * The point is: the oracle is TQBF (PSPACE-complete), DPLL solves SAT). */
    return ph_dpll_sat(f);
}

/* ================================================================
 * Sigma_2 Oracle: NP^{SAT}
 *
 * The NP machine guesses a string x, then queries a SAT oracle:
 *   "Is there a y such that NOT phi(x,y)?"
 * If oracle says NO, then forall y: phi(x,y) holds, accept.
 * ================================================================ */

/* NP^{SAT} simulation for Sigma_2.
 * Given a QBF exists x forall y: phi(x,y),
 *   For each guess x:
 *     Encode "exists y: NOT phi(x,y)" as a SAT query
 *     If oracle says UNSAT (= no y makes NOT phi true),
 *     then forall y: phi(x,y) holds, accept.
 *
 * Returns 1 if Sigma_2 formula is true, 0 otherwise. */
int ph_sigma2_oracle_solve(const PH_QBF* qbf) {
    if (!qbf) return 0;
    /* Must have at least 2 blocks for proper Sigma_2 */
    if (qbf->n_blocks < 2) return ph_sigma_k_sat(qbf);

    int nx = qbf->blocks[0].n_vars;
    int ny = 0;
    for (int i = 1; i < qbf->n_blocks; i++)
        ny += qbf->blocks[i].n_vars;

    int* x_assign = calloc((size_t)nx, sizeof(int));
    if (!x_assign) return 0;

    long long max_x = 1LL << nx;
    if (max_x > 100000) max_x = 100000;

    printf("  [NP^{SAT}: enumerating %lld x-assignments]\n", max_x);

    for (long long xm = 0; xm < max_x; xm++) {
        for (int i = 0; i < nx; i++)
            x_assign[i] = (int)((xm >> i) & 1);

        /* Build SAT query: "exists y: NOT phi(x,y)"
         * Negate the matrix: we need a formula that is SAT iff
         * there exists y falsifying phi(x_assign, y).
         *
         * NOT phi = OR over clauses of NOT(clause)
         * Each NOT(clause) = AND of negated literals.
         * But we need CNF for SAT solver. Use Tseitin or
         * just check all y via enumeration (small instances). */

        /* Simplify: check if phi(x_assign, y) is true for ALL y.
         * Forall y: phi  <=>  NOT (exists y: NOT phi)
         * So we query SAT: "is there y falsifying phi?"
         * If UNSAT, forall holds. */

        /* Build restricted formula with y-variables only */
        PH_CNF* restricted = malloc(sizeof(PH_CNF));
        if (!restricted) continue;
        memset(restricted, 0, sizeof(PH_CNF));
        restricted->n_vars = ny;

        /* Copy and evaluate x-literals */
        for (int ci = 0; ci < qbf->matrix.n_clauses; ci++) {
            PH_Clause orig = qbf->matrix.clauses[ci];
            PH_Clause simplified;
            int sl = 0, clause_sat = 0;

            for (int j = 0; j < 3; j++) {
                int lit = orig.lits[j];
                if (lit < 0) continue;
                int var = lit >> 1;
                int sign = lit & 1;

                if (var < nx) {
                    /* x-variable */
                    if ((!sign && x_assign[var]) || (sign && !x_assign[var])) {
                        clause_sat = 1; break;
                    }
                } else {
                    /* y-variable: remap */
                    int new_var = var - nx;
                    if (new_var < ny) {
                        simplified.lits[sl++] = (new_var << 1) | sign;
                    }
                }
            }

            if (!clause_sat && sl > 0) {
                while (sl < 3) simplified.lits[sl++] = -1;
                if (restricted->n_clauses < PH_MAX_CLAUSES) {
                    restricted->clauses[restricted->n_clauses++] = simplified;
                }
            }
        }

        /* Query SAT: is restricted satisfiable?
         * If yes => exists y falsifying phi => forall fails for this x.
         * If no  => forall holds => accept! */
        int sat_result = ph_dpll_sat(restricted);
        free(restricted);

        if (!sat_result) {
            /* UNSAT: no y falsifies phi => forall holds => accept */
            free(x_assign);
            return 1;
        }
    }
    free(x_assign);
    return 0;
}

/* ================================================================
 * Sigma_3 Oracle: NP^{Sigma_2}
 *
 * NP^{NP^{SAT}}: two levels of oracle nesting.
 * The NP machine guesses, then queries a Sigma_2 oracle.
 * ================================================================ */

int ph_sigma3_oracle_solve(const PH_QBF* qbf) {
    if (!qbf) return 0;
    if (qbf->n_blocks < 3) return ph_sigma2_oracle_solve(qbf);

    /* For Sigma_3: exists x forall y exists z: phi(x,y,z)
     * NP^{Sigma_2}: guess x, ask Sigma_2 oracle about rest. */
    int nx = qbf->blocks[0].n_vars;
    int remaining_blocks = qbf->n_blocks - 1;
    int remaining_vars = qbf->total_vars - nx;

    int* x_assign = calloc((size_t)nx, sizeof(int));
    if (!x_assign) return 0;

    long long max_x = 1LL << nx;
    if (max_x > 50000) max_x = 50000;

    printf("  [NP^{Sigma_2}: enumerating %lld x-assignments]\n", max_x);

    for (long long xm = 0; xm < max_x; xm++) {
        for (int i = 0; i < nx; i++)
            x_assign[i] = (int)((xm >> i) & 1);

        /* Build Sigma_2 formula for remaining variables */
        PH_QuantType* types = malloc((size_t)remaining_blocks * sizeof(PH_QuantType));
        int* sizes = malloc((size_t)remaining_blocks * sizeof(int));
        for (int i = 0; i < remaining_blocks; i++) {
            types[i] = qbf->blocks[i + 1].type;
            sizes[i] = qbf->blocks[i + 1].n_vars;
        }

        /* Restrict matrix by x_assign */
        PH_CNF* restricted = malloc(sizeof(PH_CNF));
        if (!restricted) { free(types); free(sizes); continue; }
        memset(restricted, 0, sizeof(PH_CNF));
        restricted->n_vars = remaining_vars;

        for (int ci = 0; ci < qbf->matrix.n_clauses; ci++) {
            PH_Clause orig = qbf->matrix.clauses[ci];
            PH_Clause simplified;
            int sl = 0, clause_sat = 0;

            for (int j = 0; j < 3; j++) {
                int lit = orig.lits[j];
                if (lit < 0) continue;
                int var = lit >> 1, sign = lit & 1;

                if (var < nx) {
                    if ((!sign && x_assign[var]) || (sign && !x_assign[var])) {
                        clause_sat = 1; break;
                    }
                } else {
                    int new_var = var - nx;
                    if (new_var < remaining_vars) {
                        simplified.lits[sl++] = (new_var << 1) | sign;
                    }
                }
            }

            if (!clause_sat && sl > 0) {
                while (sl < 3) simplified.lits[sl++] = -1;
                if (restricted->n_clauses < PH_MAX_CLAUSES) {
                    restricted->clauses[restricted->n_clauses++] = simplified;
                }
            }
        }

        PH_QBF* inner_qbf = ph_qbf_create(remaining_blocks, types, sizes, restricted);
        if (!inner_qbf) { free(types); free(sizes); continue; }

        /* Query Sigma_2 oracle on inner formula */
        int oracle_answer = ph_sigma2_oracle_solve(inner_qbf);
        ph_qbf_free(inner_qbf);
        free(types);
        free(sizes);

        if (oracle_answer) {
            /* Sigma_2 oracle says YES => forall y exists z: phi => accept */
            free(x_assign);
            return 1;
        }
    }
    free(x_assign);
    return 0;
}

/* ================================================================
 * Generic Oracle Solver
 *
 * Given a QBF and a level k, solve using the oracle hierarchy.
 * This demonstrates the equivalence between quantifier and oracle
 * definitions of the polynomial hierarchy.
 * ================================================================ */

int ph_oracle_solve(const PH_QBF* qbf, int level) {
    if (!qbf) return 0;
    switch (level) {
        case 0: return ph_dpll_sat(&qbf->matrix);
        case 1: return ph_sigma_k_sat(qbf);
        case 2: return ph_sigma2_oracle_solve(qbf);
        case 3: return ph_sigma3_oracle_solve(qbf);
        default: return ph_qbf_solve(qbf);
    }
}

/* Oracle demo is implemented in ph_demo.c */
