/*
 * cook_levin.h — Cook-Levin Theorem: SAT is NP-Complete
 *
 * L4 Fundamental Law:
 *   For every language L in NP, there exists a polynomial-time
 *   computable function f such that for all strings w:
 *       w in L  <=>  f(w) in SAT
 *
 * The Proof uses the TABLEAU METHOD:
 *   Given NTM M deciding L in time p(n) and input w of length n,
 *   construct CNF formula phi_{M,w} such that:
 *     phi_{M,w} is satisfiable  iff  M accepts w within p(n) steps.
 *
 * Variable Encoding (Sipser-style):
 *   cell[t][p][s] : at time t, position p contains symbol s
 *   head[t][p]    : at time t, head is at position p
 *   state[t][q]   : at time t, machine is in state q
 *
 * Clause Groups (Arora-Barak section 2.3):
 *   G1: Cell consistency — exactly one symbol per cell
 *   G2: Initial configuration + head/state uniqueness
 *   G3: Transition validity (local window rule)
 *   G4: Acceptance — machine reaches accept state
 *
 * Reference:
 *   Cook (STOC 1971) — original proof
 *   Levin (1973) — independent discovery, universal search
 *   Sipser section 7.4 — elegant tableau construction
 *   Arora-Barak section 2.3 — modern presentation
 *
 * Courses: All nine institutions
 */

#ifndef COOK_LEVIN_H
#define COOK_LEVIN_H

#include "turing_machine.h"
#include "sat.h"

/* ================================================================
 * Variable Encoding Scheme
 * ================================================================ */

/*
 * The tableau has T+1 rows (time 0..T) and P columns (positions 0..P-1).
 * Three variable families map tableau positions to Boolean variables:
 */
typedef struct {
    int T, P;           /* tableau dimensions */
    int n_states;       /* |Q| */
    int n_symbols;      /* |Gamma| (tape alphabet) */
    int cell_offset;    /* cell[t][p][s] variable base */
    int head_offset;    /* head[t][p] variable base */
    int state_offset;   /* state[t][q] variable base */
    int total_vars;     /* total Boolean variables */
} CLVariableScheme;

/* Create variable scheme for given tableau dimensions */
CLVariableScheme cl_make_var_scheme(int T, int P, int n_states, int n_symbols);

/* Variable index for cell[t][p][s] (symbol s at time t, position p) */
int cl_var_cell(const CLVariableScheme* vs, int t, int p, int s);

/* Variable index for head[t][p] (head at position p at time t) */
int cl_var_head(const CLVariableScheme* vs, int t, int p);

/* Variable index for state[t][q] (state q at time t) */
int cl_var_state(const CLVariableScheme* vs, int t, int q);

void cl_print_var_scheme(const CLVariableScheme* vs);

/* ================================================================
 * Clause Group Generators
 * ================================================================ */

/* G1: Each cell (t,p) has exactly one symbol */
void cl_add_cell_consistency(CNF* cnf, const CLVariableScheme* vs);

/* G2a: Initial configuration = input w + blank padding */
void cl_add_initial_config(CNF* cnf, const CLVariableScheme* vs,
                            const Symbol* input, int input_len,
                            int q0, int blank_idx);

/* G2b: Exactly one head position at each time step */
void cl_add_head_uniqueness(CNF* cnf, const CLVariableScheme* vs);

/* G2c: Exactly one state at each time step */
void cl_add_state_uniqueness(CNF* cnf, const CLVariableScheme* vs);

/* G3: Transition validity — local window rules */
void cl_add_transition_clauses(CNF* cnf, const CLVariableScheme* vs,
                                const TuringMachine* tm, int blank_idx);

/* G4: Machine eventually reaches accept state */
void cl_add_acceptance(CNF* cnf, const CLVariableScheme* vs, int q_accept);

/* ================================================================
 * Full Cook-Levin Reduction
 * ================================================================ */

/*
 * Complete reduction: NTM M + input w -> CNF formula phi
 * phi is satisfiable iff M accepts w within T steps.
 *
 * Complexity: O(T^2 * |Gamma|^3 * |Q|) clauses
 *            O(T^2 * |Gamma| + T * P + T * |Q|) variables
 */
CNF* cook_levin_reduce(const TuringMachine* tm,
                        const Symbol* input, int input_len, int T);

/* Detailed reduction with statistics */
typedef struct {
    CNF*  formula;
    int   n_vars, n_clauses;
    int   n_g1, n_g2, n_g3, n_g4;
    double gen_time_ms;
} CLReduction;

CLReduction* cook_levin_reduce_detailed(const TuringMachine* tm,
                                         const Symbol* input, int input_len,
                                         int T);
void cl_reduction_free(CLReduction* red);

/* ================================================================
 * Verification & Path Extraction
 * ================================================================ */

/* Verify both directions of the reduction */
int cook_levin_verify(const TuringMachine* tm,
                      const Symbol* input, int input_len, int T);

/* Extract accepting computation path from SAT assignment */
TMComputationPath* cook_levin_extract_path(const CLVariableScheme* vs,
                                            const Assignment assign,
                                            const TuringMachine* tm,
                                            const Symbol* input,
                                            int input_len, int T);

/* ================================================================
 * Polynomial Bound Analysis
 * ================================================================ */

typedef struct {
    int    degree;
    double coefficient;
    int    bound;
} PolyBound;

PolyBound cl_compute_poly_bound(int input_len, int degree, double coeff);

typedef struct {
    long long est_vars, est_clauses, est_memory_bytes;
} CLSizeEstimate;

CLSizeEstimate cl_estimate_size(int T, int P, int n_states, int n_symbols);

/* ================================================================
 * Specialized Reductions (proving NP-completeness of other problems)
 * ================================================================ */

/* SAT -> 3SAT reduction (equisatisfiable) */
CNF* sat_to_3sat(const CNF* cnf);

/* 3SAT -> CLIQUE reduction (Karp 1972) */
typedef struct {
    int  n_vertices;
    int* adj_matrix;        /* flattened n*n, 1=edge */
    int* vertex_to_literal;
    int* vertex_to_clause;
} Graph;

Graph* sat3_to_clique(const CNF* cnf_3sat);
void   graph_free(Graph* g);
int    graph_has_k_clique(const Graph* g, int k, int* clique_vertices);
void   graph_print(const Graph* g);

/* 3SAT -> VERTEX-COVER reduction */
typedef struct {
    int  n_vertices;
    int* adj_matrix;
    int  n_edges;
    int* edge_u;
    int* edge_v;
} VCGraph;

VCGraph* sat3_to_vertex_cover(const CNF* cnf_3sat);
void     vcgraph_free(VCGraph* g);
int      vcgraph_has_vertex_cover(const VCGraph* g, int k, int* cover);

#endif /* COOK_LEVIN_H */
