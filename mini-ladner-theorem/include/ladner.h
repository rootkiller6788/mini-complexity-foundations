#ifndef LADNER_H
#define LADNER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================
 * L1: Core Definitions -- Ladner's Theorem
 *
 * Ladner (JACM 1975): If P != NP, there exist NP-intermediate languages.
 * I.e., languages in NP that are NOT in P and NOT NP-complete.
 *
 * Construction: L = {x in SAT | f(|x|) is even}
 * where f is a "slow-growing" constructive function.
 * f(n) = O(log* n) which grows slower than any computable unbounded
 * monotone function.
 * ================================================================ */

/* L3: Mathematical Structures */

/* P-machine: a deterministic polynomial-time Turing machine */
typedef int (*PMachine)(int);

typedef struct {
    PMachine decide;      /* decision function */
    int    id;            /* machine identifier */
    double poly_exp;      /* O(n^k) exponent */
    int    time_bound;    /* max steps */
} P_Machine;

/* NP-machine: nondeterministic poly-time machine */
typedef int (*NPMachine)(int, int);  /* input, witness */

typedef struct {
    NPMachine verify;     /* verifier M(x,w) */
    int       id;
    double    poly_exp;
} NP_Machine;

/* NP-intermediate language */
typedef struct {
    const char* name;
    int in_np; int not_in_p; int not_npc;
    const char* hardness_desc;
    const char* non_npc_desc;
} NPIntermediate_Language;

/* Slow-growing function for Ladner's construction */
typedef struct {
    const char* name;
    int (*eval)(int n);
    int is_unbounded;
    int slower_than_poly;
    double growth_rate;
} SlowGrowingFunction;

/* Sparse set: S with |S cap {0,1}^n| le p(n) for some polynomial p */
typedef struct {
    int** data; int* sizes;
    int max_n; int poly_deg;
} SparseSet;

/* Poly-time reduction */
typedef struct {
    const char* from; const char* to;
    int (*map)(int);
    double poly_exp; int verified;
} PolyTimeReduction;

/* Oracle TM */
typedef int (*Oracle)(int);
typedef struct {
    Oracle oracle; int queries; double time_bound;
} OracleTM;

/* Poly-isomorphism */
typedef int (*Morphism)(int);
typedef struct {
    Morphism forward; Morphism backward;
    int domain_size; int verified;
} PolyIsomorphism;

/* Padded SAT instance */
typedef struct {
    int phi_index; int pad_length;
    int is_valid; int sat_value;
} PaddedSATInstance;

/* NP Degree */
typedef struct {
    const char* name; int level;
    int is_low; int is_high;
    const char* example; const char* oracle_strength;
} NP_Degree;

/* Delayed Diagonalization State */
typedef struct {
    int n; int f_n; int machine_idx;
    int next_f_change; double progress;
} DelayedDiagState;

/* CNF Formula */
typedef struct {
    int num_vars; int num_clauses;
    int** clauses; int* clause_sizes;
} CNFFormula;

/* Graph */
typedef struct {
    int n; int** adj; char* label;
} Graph;

/* NP-intermediate candidate record */
typedef struct {
    const char* name; const char* description;
    const char* best_known; int year;
    int likely_npi; const char* crypto_relevance;
} NPICandidate;

/* ================================================================
 * L4: Core Function API -- Ladner's Construction
 * ================================================================ */

int  slow_growing_f(int n);
int  slow_growing_popcount(int n);
int  slow_growing_inv_ack(int n);
int  slow_growing_eval(int n, int f_type);
int  padded_sat_member(int n);
int  padded_sat_decode(int n, int* phi_len, int* pad_len);
int  ladner_language_member(int* input, int len);
void ladner_construction_demo(void);
int  delayed_diagonal(int n);
void delayed_diag_demo(void);

/* L4: Supporting Theorems */
int  mahaney_sparse_check(SparseSet* S);
long long mahaney_prune(int n_vars, int sparsity_deg);
void mahaney_demo(void);
int  berman_hartmanis_verify_iso(PolyIsomorphism* iso, int (*inA)(int), int (*inB)(int));
int  berman_hartmanis_check(void);
void berman_hartmanis_demo(void);
SparseSet* sparse_new(int max_n);
int  sparse_add(SparseSet* s, int n, int val);
int  sparse_contains(SparseSet* s, int n, int val);
void sparse_free(SparseSet* s);
int  sparse_is_bound(SparseSet* s, int poly_deg);
void sparse_sets_demo(void);
int  deg_cmp(NP_Degree a, NP_Degree b);
int  deg_interp(int lo, int hi);
void degree_structure_demo(void);

/* L5/L6: Algorithms & Canonical Problems */
long long gcd_ll(long long a, long long b);
long long mod_mul(long long a, long long b, long long m);
long long pollard_rho(long long n);
long long fermat_factor(long long n);
void trial_division(long long n, long long* f1, long long* f2);
void factor_full(long long n, long long* f1, long long* f2);
void factoring_demo(void);
Graph* g_new(int n);
void g_free(Graph* g);
int  g_add_edge(Graph* g, int u, int v);
Graph* g_permute(Graph* g, int* p);
int  g_equal(Graph* a, Graph* b);
int  gi_brute(Graph* a, Graph* b);
int  wl_test(Graph* a, Graph* b, int iters);
void deg_seq(Graph* g, int* degs);
void graph_isomorphism_demo(void);
void padded_sat_full_demo(void);
int  padded_sat_verify(int n);
int  count_solutions(int nv, int (*eval)(int));
int  vv_reduce(int nv, int (*eval)(int), int* nc);
void uniqueness_demo(void);
int  is_square_free(int n);
int  is_quadratic_residue(int n, int a);
int  deg_sequence_match(int* g1, int* g2, int n_g);
void promise_problems_demo(void);
void np_intermediate_candidates_demo(void);
int  npi_candidate_count(void);

/* L7/L8: Advanced Topics */
void schoning_hierarchy_demo(int max_k, int n_vars);
double schoning_density_estimate(int n_vars, int instance_count);
int  schoning_degree_compare(double pf_a, double pf_b);
void schoning_demo(void);
int  has_high_circuit_complexity(int circuit_size, int* truth_table, int n_vars);
void razborov_rudich_demo(void);
void natural_proofs_demo(void);
void relativized_ladner_demo(void);
void relativization_demo(void);
int  oracle_relativized_P(PMachine m, Oracle O, int input);
int  oracle_relativized_NP(NPMachine m, Oracle O, int input, int witness);
void oracle_construction_demo(void);

/* Utility & Benchmark */
void ladner_bench(void);
void sparse_operations_demo(void);
void ladner_language_demo(void);
int  ladner_membership(int n);
int  ladner_f(int n);

/* ================================================================
 * L9: Research Frontiers (documentation pointers)
 *   - Meta-complexity: is Ladner's L "natural"?
 *   - GI quasipolynomial-time (Babai 2015)
 *   - Quantum complexity: Shor vs NP-intermediate
 *   - GCT: algebraic approach to P vs NP
 * ================================================================ */

#ifdef __cplusplus
}
#endif

#endif /* LADNER_H */
