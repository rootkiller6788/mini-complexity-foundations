/* ph.h — Polynomial Hierarchy: Complete Header
 *
 * PH = U_{k>=0} Sigma_k^p = U_{k>=0} Pi_k^p
 *
 * Sigma_0^p = Pi_0^p = P
 * Sigma_{k+1}^p = NP^{Sigma_k^p}
 * Pi_k^p = co-Sigma_k^p
 *
 * Core references:
 *   Stockmeyer (1976) — definition of PH
 *   Arora & Barak, Chapter 5
 *   Sipser, Chapter 9 (oracle machines, PH)
 *
 * Key theorems implemented:
 *   L4.1  Sigma_k-SAT is Sigma_k-complete (Stockmeyer 1976)
 *   L4.2  Sigma_k=Pi_k => PH=Sigma_k (collapse)
 *   L4.3  NP in P/poly => PH=Sigma_2 (Karp-Lipton 1982)
 *   L4.4  PH subseteq P^{#P} (Toda 1991)
 *   L4.5  BPP subseteq Sigma_2 cap Pi_2 (Sipser-Gacs-Lautemann 1983)
 *   L4.6  Sigma_k = ATIME(poly(n), k alternations) (CKS 1981)
 */
#ifndef PH_H
#define PH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

/* ===================================================================
 * Polynomial Hierarchy (PH) — Complete Implementation
 *
 * Σ₀^p = Π₀^p = P
 * Σ_{k+1}^p = NP^{Σ_k^p}
 * Π_k^p = co-Σ_k^p
 * PH = ∪_{k≥0} Σ_k^p = ∪_{k≥0} Π_k^p
 *
 * Two equivalent definitions:
 *   1. Quantifier alternation: Σ_k = {L | ∃x₁∀x₂...Q_k x_k: R(x₁,...,x_k)}
 *      where R is polynomial-time decidable.
 *   2. Oracle hierarchy: Σ_{k+1} = NP^{Σ_k}
 *
 * Key structural results:
 *   - PH ⊆ PSPACE (via TQBF)
 *   - If Σ_k = Π_k then PH collapses to Σ_k
 *   - If P = NP then PH = P (collapse to level 0)
 *   - It is unknown whether PH is infinite (≠ P ≠ NP)
 *
 * Core references:
 *   Stockmeyer, L. "The Polynomial-Time Hierarchy." TCS 3(1):1-22, 1976.
 *   Meyer, A. and Stockmeyer, L. "The Equivalence Problem for Regular
 *     Expressions with Squaring Requires Exponential Space." FOCS 1972.
 *   Arora, S. and Barak, B. "Computational Complexity: A Modern Approach."
 *     Cambridge University Press, 2009. Chapter 5.
 *   Sipser, M. "Introduction to the Theory of Computation." 3rd ed.
 *     Cengage Learning, 2013. Chapter 9.
 * =================================================================== */

#define PH_MAX_LEVEL    6
#define PH_MAX_VARS     32
#define PH_MAX_CLAUSES  2048
#define PH_MAX_GATES    256
#define PH_MAX_INPUTS   32

/* ---------- L1.1: CNF Formula ---------- */
/* Literal encoding: (var_index << 1) | sign_bit
 *   sign_bit = 0 => positive (x_var)
 *   sign_bit = 1 => negative (not x_var)
 * negated var or index >= PH_MAX_VARS => padding sentinel */
typedef struct {
    int lits[3];
} PH_Clause;

typedef struct {
    int n_vars;
    int n_clauses;
    PH_Clause clauses[PH_MAX_CLAUSES];
} PH_CNF;

/* ---------- L1.2: Quantifier Types ---------- */
typedef enum {
    PH_QUANT_EXISTS = 0,
    PH_QUANT_FORALL = 1
} PH_QuantType;

typedef struct {
    PH_QuantType type;
    int start_var;
    int n_vars;
} PH_QuantBlock;

/* ---------- L1.3: QBF (Quantified Boolean Formula) ---------- */
typedef struct {
    int n_blocks;
    int total_vars;
    PH_QuantBlock blocks[PH_MAX_LEVEL];
    PH_CNF matrix;
} PH_QBF;

/* ---------- L1.4: Boolean Circuit ---------- */
typedef enum {
    PH_GATE_AND   = 0,
    PH_GATE_OR    = 1,
    PH_GATE_NOT   = 2,
    PH_GATE_INPUT = 3,
    PH_GATE_CONST = 4
} PH_GateType;

typedef struct {
    PH_GateType type;
    int in1, in2;
    int const_val;
} PH_Gate;

typedef struct {
    int n_inputs;
    int n_gates;
    PH_Gate gates[PH_MAX_GATES];
} PH_BoolCircuit;

/* ---------- L1.5: Oracle Machine ---------- */
typedef int (*PH_Oracle)(int query);

typedef struct {
    int  input;
    int  witness_len;
    int  query_len;
    int  n_queries;
    int* query_results;
} PH_OracleMachine;

/* ---------- L1.6: Alternating TM State ---------- */
typedef enum {
    PH_ATM_EXISTS = 0,
    PH_ATM_FORALL = 1,
    PH_ATM_ACCEPT = 2,
    PH_ATM_REJECT = 3
} PH_ATMState;

typedef struct {
    PH_ATMState type;
    int n_transitions;
    int transitions[4];
} PH_ATMConfig;

/* ===================================================================
 * L2-L3: CNF Operations
 * =================================================================== */

/* CNF operations */
int  ph_cnf_eval(const PH_CNF* f, const int* assign);
PH_CNF* ph_cnf_parse(const char* desc);
PH_CNF* ph_cnf_build_single(int v1, int s1, int v2, int s2, int v3, int s3);
PH_CNF* ph_cnf_clone(const PH_CNF* src);
void ph_cnf_print(const PH_CNF* f);
int  ph_cnf_is_tautology(const PH_CNF* f);
void ph_cnf_to_dnf(const PH_CNF* f, PH_CNF* dnf_out);
int  ph_cnf_equivalent(const PH_CNF* f1, const PH_CNF* f2);
void ph_cnf_var_frequency(const PH_CNF* f, int occ_count[][2]);

/* ===================================================================
 * L3-L5: Sigma_k / Pi_k SAT Solvers
 * =================================================================== */

int ph_sigma_k_sat(const PH_QBF* qbf);
int ph_pi_k_sat(const PH_QBF* qbf);
int ph_qbf_solve(const PH_QBF* qbf);

/* ===================================================================
 * QBF Construction and Manipulation
 * =================================================================== */

PH_QBF* ph_qbf_create(int n_blocks, const PH_QuantType* types,
                      const int* sizes, PH_CNF* f);
void    ph_qbf_free(PH_QBF* qbf);
void    ph_qbf_print(const PH_QBF* qbf);
PH_QBF* ph_qbf_restrict(const PH_QBF* qbf, const int* assign);

/* ===================================================================
 * L4: Key Theorems
 * =================================================================== */

/* Collapse Theorem */
int ph_collapse_check(int max_level);

/* Karp-Lipton: Circuit Operations */
int  ph_circuit_eval(const PH_BoolCircuit* c, const int* inputs);
void ph_circuit_print(const PH_BoolCircuit* c);
PH_BoolCircuit* ph_circuit_build_and(int n_inputs);
PH_BoolCircuit* ph_circuit_build_or(int n_inputs);
PH_BoolCircuit* ph_circuit_build_majority(int n_inputs);

/* Toda's Theorem: #SAT and counting-based simulation */
long long ph_count_sat(const PH_CNF* f);
int ph_sigma2_via_counting(const PH_QBF* qbf);

/* BPP in Sigma_2 (Sipser-Gacs-Lautemann) */
typedef int (*PH_BPPPredict)(int input, int random_bits);
int ph_bpp_simulate(PH_BPPPredict pred, int input, int n_random,
                    int n_trials, double* error_bound);
int ph_bpp_to_sigma2(PH_BPPPredict pred, int input, int n_witness);

/* Alternating TM */
int ph_atm_simulate(const PH_ATMConfig* config, int n_states,
                    int start_state, int depth, int max_depth,
                    int is_exist, int input);

/* ===================================================================
 * L5: Algorithm Implementations
 * =================================================================== */

int  ph_dpll_sat(const PH_CNF* f);
void ph_sat_to_clique(const PH_CNF* f, int* graph_out,
                      int* n_out, int* k_out);
void ph_sat_to_vertexcover(const PH_CNF* f, int* graph_out,
                           int* n_out, int* k_out);

/* Additional Karp reductions (L5) */
void ph_sat_to_3color(const PH_CNF* f, int* graph_out, int* n_out);
void ph_sat_to_subsetsum(const PH_CNF* f, int* numbers_out,
                         int* n_out, int* target_out);
void ph_sat_to_hampath(const PH_CNF* f, int* graph_out, int* n_out);
int  ph_verify_sat_to_clique(void);
int  ph_verify_sat_to_vertexcover(void);
void ph_demo_reductions(void);

/* Circuit construction and transformation (L4-L5) */
PH_BoolCircuit* ph_circuit_build_xor(void);
PH_BoolCircuit* ph_circuit_build_iff(int n_inputs);
PH_BoolCircuit* ph_circuit_build_threshold(int n_inputs, int k);
PH_BoolCircuit* ph_circuit_build_parity(int n_inputs);
int  ph_circuit_count_gates(const PH_BoolCircuit* c);
int  ph_circuit_depth(const PH_BoolCircuit* c);
void ph_cnf_to_circuit(const PH_CNF* f, PH_BoolCircuit* c);

/* Oracle machine simulation (L4-L5) */
PH_OracleMachine* ph_oracle_create(int input, int witness_len, int query_len);
void ph_oracle_free(PH_OracleMachine* m);
int  ph_sigma1_oracle_solve(const PH_CNF* f);
int  ph_sigma2_oracle_solve(const PH_QBF* qbf);
int  ph_sigma3_oracle_solve(const PH_QBF* qbf);
int  ph_oracle_solve(const PH_QBF* qbf, int level);
void ph_demo_oracle_full(void);

/* ===================================================================
 * L6: Canonical Complete Problems
 * =================================================================== */

PH_QBF* ph_qsat_benchmark(int k, int vars_per_block);
int     ph_tqbf_solve(const PH_QBF* qbf);
int     ph_circuit_value(const PH_BoolCircuit* c, const int* inputs);

/* ===================================================================
 * L7: Applications
 * =================================================================== */

int ph_bounded_model_check(int n_states, int n_steps,
                           const int* transition, const int* error_states);
int ph_game_solver(const PH_QBF* game_qbf, int* winning_strategy);
int ph_circuit_minimization_check(const PH_BoolCircuit* c, int target_gates);

/* ===================================================================
 * L8: Advanced Topics
 * =================================================================== */

int ph_pcp_verify(const char* proof, int proof_len,
                  int q_queries, int r_random_bits,
                  int (*verifier)(const char*, int*));
int ph_valiant_vazirani(const PH_CNF* f, PH_CNF* isolated_out);

/* ===================================================================
 * L9: Research Frontiers
 * =================================================================== */

void ph_research_summary(void);

/* ===================================================================
 * Structural / Demo Functions
 * =================================================================== */

void ph_print_hierarchy(void);
void ph_demo_stockmeyer(void);
void ph_demo_toda(void);
void ph_demo_karp_lipton(void);
void ph_demo_bpp(void);
void ph_demo_collapse(void);
void ph_demo_oracle(void);
void ph_demo_atm(void);
void ph_demo_complete(void);
void ph_demo_qbf(void);
void ph_benchmark(void);
void ph_summary(void);

/* ===================================================================
 * Benchmarking
 * =================================================================== */

void ph_bench_sigma_k(void);

/* ===================================================================
 * Interactive Proofs (L8 Advanced)
 * =================================================================== */

/* IP = PSPACE (Shamir 1992): interactive proofs capture PSPACE.
 * Arithmetization + sumcheck protocol verifies TQBF interactively. */
int  ph_ip_verify_tqbf(const PH_QBF* qbf, int n_rounds);
int  ph_mip_verify(int (*prover1)(int), int (*prover2)(int),
                   int n_questions, int* accept_count);
void ph_demo_ip(void);

#endif /* PH_H */