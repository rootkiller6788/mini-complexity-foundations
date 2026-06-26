/*
 * tableau.c — Cook-Levin Tableau Construction
 *
 * L4 Fundamental Law Implementation:
 *   The tableau method is THE proof of the Cook-Levin theorem.
 *   It constructs, for any NTM M and input w, a CNF formula phi
 *   such that phi is satisfiable iff M accepts w within T steps.
 *
 * The tableau is a T x P grid where:
 *   - Row t represents the configuration at time t
 *   - Column p represents tape position p
 *   - Cells encode (symbol, head_presence, state) triples
 *
 * Variable Scheme (Sipser-style, simplified):
 *   cell[t][p][s]    — tape cell at (t,p) holds symbol s
 *   head[t][p]       — head at (t,p)
 *   state[t][q]      — machine in state q at time t
 *
 * Clause Groups (Arora-Barak section 2.3):
 *   G1: Cell consistency — exactly one symbol per cell
 *   G2: Initial configuration + uniqueness constraints
 *   G3: Transition validity (window rule)
 *   G4: Acceptance clause
 *
 * Theorem (Cook-Levin):
 *   The resulting CNF formula has size O(T^2 * |Gamma|^2) and
 *   is satisfiable iff M accepts w within T steps.
 *
 * Reference:
 *   Cook (STOC 1971), Levin (1973)
 *   Sipser section 7.4, Arora-Barak section 2.3
 *   Garey & Johnson section 2.6
 *
 * Courses: All nine institutions
 */

#include "cook_levin.h"
#include "turing_machine.h"
#include "sat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ================================================================
 * Variable Encoding Scheme
 * ================================================================ */

CLVariableScheme cl_make_var_scheme(int T, int P, int n_states, int n_symbols) {
    CLVariableScheme vs;
    memset(&vs, 0, sizeof(vs));
    vs.T = T;
    vs.P = P;
    vs.n_states = n_states;
    vs.n_symbols = n_symbols;

    /*
     * Variable layout (0-indexed):
     *   Variable 0 is reserved (unused, or use 1-indexed).
     *   We use 0-indexed internally, but the SAT solver uses 0..total_vars-1.
     *
     *   cell[t][p][s]:  (T+1) * P * n_symbols variables
     *     offset = 0
     *     index = t * P * n_symbols + p * n_symbols + s
     *
     *   head[t][p]: (T+1) * P variables
     *     offset = (T+1) * P * n_symbols
     *     index = offset + t * P + p
     *
     *   state[t][q]: (T+1) * n_states variables
     *     offset = (T+1) * P * n_symbols + (T+1) * P
     *     index = offset + t * n_states + q
     */
    vs.cell_offset = 0;
    vs.head_offset = (T + 1) * P * n_symbols;
    vs.state_offset = vs.head_offset + (T + 1) * P;
    vs.total_vars = vs.state_offset + (T + 1) * n_states;

    return vs;
}

int cl_var_cell(const CLVariableScheme* vs, int t, int p, int s) {
    if (!vs || t < 0 || t > vs->T || p < 0 || p >= vs->P || s < 0 || s >= vs->n_symbols)
        return -1;
    return vs->cell_offset + t * vs->P * vs->n_symbols + p * vs->n_symbols + s;
}

int cl_var_head(const CLVariableScheme* vs, int t, int p) {
    if (!vs || t < 0 || t > vs->T || p < 0 || p >= vs->P)
        return -1;
    return vs->head_offset + t * vs->P + p;
}

int cl_var_state(const CLVariableScheme* vs, int t, int q) {
    if (!vs || t < 0 || t > vs->T || q < 0 || q >= vs->n_states)
        return -1;
    return vs->state_offset + t * vs->n_states + q;
}

void cl_print_var_scheme(const CLVariableScheme* vs) {
    if (!vs) return;
    printf("Cook-Levin Variable Scheme:\n");
    printf("  T=%d, P=%d, |Q|=%d, |Gamma|=%d\n",
           vs->T, vs->P, vs->n_states, vs->n_symbols);
    printf("  cell vars:  %d (offset %d)\n",
           (vs->T + 1) * vs->P * vs->n_symbols, vs->cell_offset);
    printf("  head vars:  %d (offset %d)\n",
           (vs->T + 1) * vs->P, vs->head_offset);
    printf("  state vars: %d (offset %d)\n",
           (vs->T + 1) * vs->n_states, vs->state_offset);
    printf("  TOTAL vars: %d\n", vs->total_vars);
}

/* ================================================================
 * Helper: At-Least-One and At-Most-One Clauses
 * ================================================================ */

/*
 * At-least-one over a set of variable indices: (v0 OR v1 OR ... OR v{n-1})
 */
static void add_at_least_one(CNF* cnf, const int* vars, int n) {
    if (n <= 0) return;
    int* lits = (int*)malloc((size_t)n * sizeof(int));
    for (int i = 0; i < n; i++) {
        lits[i] = SAT_LIT_POS(vars[i]);  /* positive literal */
    }
    cnf_add_clause(cnf, lits, n);
    free(lits);
}

/*
 * At-most-one over a set of variable indices.
 * For each pair (i, j) with i < j: (¬vi OR ¬vj)
 */
static void add_at_most_one(CNF* cnf, const int* vars, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int lits[2] = { SAT_LIT_NEG(vars[i]), SAT_LIT_NEG(vars[j]) };
            cnf_add_clause(cnf, lits, 2);
        }
    }
}

/*
 * Exactly-one = at-least-one + at-most-one.
 */
static void add_exactly_one(CNF* cnf, const int* vars, int n) {
    add_at_least_one(cnf, vars, n);
    add_at_most_one(cnf, vars, n);
}

/* ================================================================
 * G1: Cell Consistency
 * ================================================================ */

void cl_add_cell_consistency(CNF* cnf, const CLVariableScheme* vs) {
    int max_symbols = vs->n_symbols;
    int* cell_vars = (int*)malloc((size_t)max_symbols * sizeof(int));

    for (int t = 0; t <= vs->T; t++) {
        for (int p = 0; p < vs->P; p++) {
            for (int s = 0; s < max_symbols; s++) {
                cell_vars[s] = cl_var_cell(vs, t, p, s);
            }
            add_exactly_one(cnf, cell_vars, max_symbols);
        }
    }
    free(cell_vars);
}

/* ================================================================
 * G2a: Initial Configuration
 * ================================================================ */

void cl_add_initial_config(CNF* cnf, const CLVariableScheme* vs,
                            const Symbol* input, int input_len,
                            int q0, int blank_idx) {
    /* Time 0: head at position 0 */
    cnf_add_unit(cnf, SAT_LIT_POS(cl_var_head(vs, 0, 0)));

    /* Time 0: state = q0 */
    cnf_add_unit(cnf, SAT_LIT_POS(cl_var_state(vs, 0, q0)));

    /* Time 0: tape contents */
    for (int p = 0; p < vs->P; p++) {
        Symbol sym;
        if (p < input_len)
            sym = input[p];
        else
            sym = BLANK_SYMBOL;

        /* Find the symbol index for 'sym' */
        int sym_idx = (sym == BLANK_SYMBOL) ? blank_idx : sym;

        cnf_add_unit(cnf, SAT_LIT_POS(cl_var_cell(vs, 0, p, sym_idx)));

        /* All other symbols are false at this position */
        for (int s = 0; s < vs->n_symbols; s++) {
            if (s != sym_idx) {
                cnf_add_unit(cnf, SAT_LIT_NEG(cl_var_cell(vs, 0, p, s)));
            }
        }
    }
}

/* ================================================================
 * G2b: Head Uniqueness
 * ================================================================ */

void cl_add_head_uniqueness(CNF* cnf, const CLVariableScheme* vs) {
    int* head_vars = (int*)malloc((size_t)vs->P * sizeof(int));

    for (int t = 0; t <= vs->T; t++) {
        for (int p = 0; p < vs->P; p++) {
            head_vars[p] = cl_var_head(vs, t, p);
        }
        add_exactly_one(cnf, head_vars, vs->P);
    }
    free(head_vars);
}

/* ================================================================
 * G2c: State Uniqueness
 * ================================================================ */

void cl_add_state_uniqueness(CNF* cnf, const CLVariableScheme* vs) {
    int* state_vars = (int*)malloc((size_t)vs->n_states * sizeof(int));

    for (int t = 0; t <= vs->T; t++) {
        for (int q = 0; q < vs->n_states; q++) {
            state_vars[q] = cl_var_state(vs, t, q);
        }
        add_exactly_one(cnf, state_vars, vs->n_states);
    }
    free(state_vars);
}

/* ================================================================
 * G3: Transition Validity (Window Rule)
 * ================================================================ */

/*
 * Encode the local transition rule.
 *
 * Key insight: The value of cell[t+1][p] depends only on:
 *   - cell[t][p-1], cell[t][p], cell[t][p+1] (symbols at positions near p)
 *   - head[t][p-1], head[t][p], head[t][p+1] (head position at time t)
 *   - state[t][q] (machine state at time t)
 *
 * This is because:
 *   - If the head is not at p-1, p, or p+1, then cell[t+1][p] = cell[t][p]
 *     (the tape doesn't change at positions far from the head)
 *   - If the head IS at p, then cell[t+1][p] is determined by the
 *     transition: cell[t+1][p] = write_symbol of the transition
 *
 * We encode this as a set of clauses for each (t, p) covering all
 * possible (symbol triple, head position, state) combinations.
 *
 * For efficiency, we only generate clauses for valid transitions.
 */
void cl_add_transition_clauses(CNF* cnf, const CLVariableScheme* vs,
                                const TuringMachine* tm, int blank_idx) {
    /*
     * For each time t and position p:
     *   If head[t][p] is true:
     *     cell[t+1][p] = write_symbol determined by transition(state[t][q], cell[t][p])
     *     head[t+1][p+dir] (head moves)
     *
     * This is encoded via the "window" method:
     *   For each possible (q, sym_p, sym_left, sym_right) combination,
     *   add a clause enforcing the correct next configuration.
     *
     * Simplified approach: For each transition delta(q,a) -> (q',a',d),
     * add clauses:
     *   (state[t][q] AND head[t][p] AND cell[t][p]=a) => cell[t+1][p]=a'
     *   (state[t][q] AND head[t][p] AND cell[t][p]=a) => head[t+1][p+d]
     *   (state[t][q] AND head[t][p] AND cell[t][p]=a) => state[t+1][q']
     */

    for (int t = 0; t < vs->T; t++) {
        for (int p = 0; p < vs->P; p++) {
            for (int tr = 0; tr < tm->num_trans; tr++) {
                TMTransition* trans = &tm->transitions[tr];
                int q = trans->from_state;
                Symbol read_sym = trans->read_symbol;
                int read_idx = (read_sym == BLANK_SYMBOL) ? blank_idx : read_sym;
                Symbol write_sym = trans->write_symbol;
                int write_idx = (write_sym == BLANK_SYMBOL) ? blank_idx : write_sym;
                int q_next = trans->to_state;

                /*
                 * Guard: state[t][q] AND head[t][p] AND cell[t][p][read_sym]
                 * Implication: cell[t+1][p][write_idx] AND state[t+1][q_next]
                 *              AND head[t+1][p + direction]
                 *
                 * Encoded as clause:
                 *   ¬state[t][q] OR ¬head[t][p] OR ¬cell[t][p][read_idx]
                 *   OR cell[t+1][p][write_idx]
                 */

                int guard_vars[3] = {
                    cl_var_state(vs, t, q),
                    cl_var_head(vs, t, p),
                    cl_var_cell(vs, t, p, read_idx)
                };

                /* cell[t+1][p] = write symbol */
                {
                    int lits[4] = {
                        SAT_LIT_NEG(guard_vars[0]),
                        SAT_LIT_NEG(guard_vars[1]),
                        SAT_LIT_NEG(guard_vars[2]),
                        SAT_LIT_POS(cl_var_cell(vs, t + 1, p, write_idx))
                    };
                    cnf_add_clause(cnf, lits, 4);
                }

                /* state[t+1] = next state */
                {
                    int lits[4] = {
                        SAT_LIT_NEG(guard_vars[0]),
                        SAT_LIT_NEG(guard_vars[1]),
                        SAT_LIT_NEG(guard_vars[2]),
                        SAT_LIT_POS(cl_var_state(vs, t + 1, q_next))
                    };
                    cnf_add_clause(cnf, lits, 4);
                }

                /* head[t+1] moves to p + direction */
                int next_p = p + (int)trans->direction;
                if (next_p >= 0 && next_p < vs->P) {
                    int lits[4] = {
                        SAT_LIT_NEG(guard_vars[0]),
                        SAT_LIT_NEG(guard_vars[1]),
                        SAT_LIT_NEG(guard_vars[2]),
                        SAT_LIT_POS(cl_var_head(vs, t + 1, next_p))
                    };
                    cnf_add_clause(cnf, lits, 4);
                }
            }
        }
    }

    /*
     * Inertia clauses: If the head is NOT at position p at time t,
     * then cell[t+1][p] = cell[t][p] for all symbols.
     *
     * Encoded as: head[t][p] OR ¬cell[t][p][s] OR cell[t+1][p][s]
     * for each (t, p, s).
     */
    for (int t = 0; t < vs->T; t++) {
        for (int p = 0; p < vs->P; p++) {
            int head_var = cl_var_head(vs, t, p);
            for (int s = 0; s < vs->n_symbols; s++) {
                int cell_t = cl_var_cell(vs, t, p, s);
                int cell_t1 = cl_var_cell(vs, t + 1, p, s);
                int lits[3] = {
                    SAT_LIT_POS(head_var),
                    SAT_LIT_NEG(cell_t),
                    SAT_LIT_POS(cell_t1)
                };
                cnf_add_clause(cnf, lits, 3);
            }
        }
    }
}

/* ================================================================
 * G4: Acceptance Clause
 * ================================================================ */

void cl_add_acceptance(CNF* cnf, const CLVariableScheme* vs, int q_accept) {
    /*
     * For some t in [0..T]: state[t][q_accept] is true.
     * state[0][q_accept] OR state[1][q_accept] OR ... OR state[T][q_accept]
     */
    int* lits = (int*)malloc((size_t)(vs->T + 1) * sizeof(int));
    for (int t = 0; t <= vs->T; t++) {
        lits[t] = SAT_LIT_POS(cl_var_state(vs, t, q_accept));
    }
    cnf_add_clause(cnf, lits, vs->T + 1);
    free(lits);
}