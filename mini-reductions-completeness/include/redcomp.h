#ifndef REDCOMP_H
#define REDCOMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* ===== Reduction Type Taxonomy (L1) =====
 *
 * A reduction is a transformation from one decision problem to another.
 * We formalize five major types, each with different oracle access models.
 *
 * Definition (Karp, 1972): A polynomial-time many-one reduction A <=_m B
 * is a function f: Sigma* -> Sigma* computable in poly(|x|) time such that
 *   x in A  iff  f(x) in B.
 *
 * Definition (Cook, 1971): A polynomial-time Turing reduction A <=_T B
 * means there is a polynomial-time oracle TM M such that M^B decides A.
 *
 * Relationship: <=_m  ⊂  <=_T  (proper subset if NP != coNP).
 *              <=_L  ⊂  <=_m  (logspace is stricter than poly-time). */

typedef enum {
    REDUCTION_MANY_ONE,   /* Karp: A <=_m B, single query, non-adaptive */
    REDUCTION_TURING,     /* Cook: A <=_T B, multiple adaptive oracle queries */
    REDUCTION_LOGSPACE,   /* A <=_L B, f computable in O(log n) space */
    REDUCTION_TRUTH_TABLE,/* A <=_tt B, list all queries, combine with Boolean fn */
    REDUCTION_BOUNDED_Q   /* A <=_{k-tt} B, at most k non-adaptive queries */
} ReductionType;

/* Formal reduction structure */
typedef struct {
    int problem_from_id;       /* Source problem ID */
    int problem_to_id;         /* Target problem ID */
    ReductionType type;        /* Reduction type */
    double time_complexity_n;  /* Exponent: O(n^k) */
    int is_parsimonious;       /* 1 if preserves solution COUNT */
    int is_witness_preserving; /* 1 if Levin reduction (witness can be recovered) */
    const char *description;   /* Human-readable description */
} Reduction;

/* ===== Problem and Class Definitions (L1, L2) ===== */

/* Complexity class designators */
typedef enum {
    CLASS_L,           /* Logarithmic space */
    CLASS_NL,          /* Nondeterministic logspace */
    CLASS_P,           /* Polynomial time */
    CLASS_NP,          /* Nondeterministic polynomial time */
    CLASS_coNP,        /* Complement of NP */
    CLASS_PSPACE,      /* Polynomial space */
    CLASS_EXP,         /* Exponential time */
    CLASS_NEXP,        /* Nondeterministic exponential time */
    CLASS_PH_SIGMA_2,  /* Second level of polynomial hierarchy */
    CLASS_PH_PI_2,     /* co-Sigma_2 */
    CLASS_PSPACE_HARD  /* Above PSPACE */
} ComplexityClass;

/* A decision problem: L ⊆ {0,1}* */
typedef struct {
    int id;                  /* Karp catalog ID or 0 for custom */
    const char *name;        /* e.g., "SAT", "3SAT", "CLIQUE" */
    const char *instance_type; /* "CNF formula", "Graph+integer", etc. */
    ComplexityClass in_class;/* The class this problem belongs to */
    int is_complete_for;     /* Index into complexity class (0=P,1=NP,...) */
    int (*decider)(void *instance, void *witness); /* Decision function */
    const char *input_encoding_scheme;
} DecisionProblem;

/* ===== Core Reduction API (L3) ===== */

/* Verify a reduction f is computable in polynomial time.
   Measures wall-clock scaling up to max_n and checks power-law fit. */
int reduction_is_polynomial(int (*f)(int), int max_n);

/* Verify a reduction: forall x up to max_n, x in A iff f(x) in B.
   This is a bounded-model check of the reduction's correctness property. */
int reduction_verify(int (*f)(int), int (*decide_A)(int),
                     int (*decide_B)(int), int max_n);

/* Compose two reductions: if A <=_m B (via f) and B <=_m C (via g),
   then A <=_m C via g ∘ f. Returns estimated time exponent. */
double reduction_compose(Reduction *r1, Reduction *r2, Reduction *result);

/* Check reduction transitivity: verify that reduction composition
   preserves the correctness property. */
int reduction_is_transitive(Reduction *ab, Reduction *bc, int max_test);

/* ===== Reduction Chain Operations (L2) ===== */

/* Maximum problems in a reduction chain */
#define MAX_CHAIN_NODES 64

typedef struct {
    DecisionProblem problems[MAX_CHAIN_NODES];
    Reduction reductions[MAX_CHAIN_NODES];
    int n_problems;
    int n_reductions;
    /* Adjacency: reductions[i][j] = reduction index from i to j, -1 if none */
    int adjacency[MAX_CHAIN_NODES][MAX_CHAIN_NODES];
} ReductionChain;

/* Initialize an empty reduction chain */
void chain_init(ReductionChain *chain);

/* Add a reduction to the chain */
int chain_add_reduction(ReductionChain *chain, Reduction *r);

/* Check if chain is transitively closed */
int chain_is_transitively_closed(const ReductionChain *chain);

/* Compute the transitive closure (all implied reductions) */
int chain_transitive_closure(ReductionChain *chain, ReductionChain *out);

/* Find all problems reachable from a given problem in the chain */
int chain_reachable(const ReductionChain *chain, int problem_id,
                    int *reachable, int max_reachable);

/* ===== Core Reductions (Karp's Chain, L5) ===== */

/* SAT -> 3SAT: any CNF to 3-CNF via auxiliary variables */
void sat_to_3sat_demo(void);

/* 3SAT -> CLIQUE: formula to graph with k-clique */
void three_sat_to_clique_demo(void);

/* 3SAT -> VERTEX COVER: k = n_vars + 2*n_clauses */
void three_sat_to_vc_demo(void);

/* 3SAT -> HAMILTONIAN CYCLE: diamond gadget construction */
void three_sat_to_hc_demo(void);

/* 3SAT -> SUBSET SUM: base-10 digit encoding */
void three_sat_to_ss_demo(void);

/* 3SAT -> 3-COLORING: variable + clause triangle gadgets */
void three_sat_to_3color_demo(void);

/* ===== Reduction Theory Demonstrations (L4, L5) ===== */

/* Cook vs Karp reductions: differences and equivalences,
   oracle separation argument. */
void cook_vs_karp_demo(void);

/* The complete Karp reduction chain: visualizes the DAG
   of 21 reductions from SAT. */
void reduction_chain_demo(void);

/* NP-hardness: definition, examples beyond NP,
   the Entscheidungsproblem connection. */
void np_hardness_demo(void);

/* Closure properties: P, NP, coNP, PSPACE, EXP, PH
   are closed under polynomial-time reductions.
   Includes formal proofs as C assertions. */
void closure_demo(void);

/* Logspace reductions: A <=_L B, applications to
   P-completeness and NL-completeness. */
void logspace_red_demo(void);

/* Catalog of Karp's 21 NP-complete problems with
   descriptions, reduction sources, and categories. */
void catalog_demo(void);

/* Benchmark reduction runtime to verify polynomial scaling */
void reduction_bench(void);

/* Fuzz test reduction correctness on random instances */
void reduction_fuzz(void);

/* ===== Self-Reducibility & Levin Reductions (L4, L5) ===== */

/* SAT is self-reducible: given an oracle for SAT,
   we can find a satisfying assignment in polynomial time. */
void self_reducibility_demo(void);

/* Levin reduction: a witness-preserving Karp reduction.
   If A <=_L^wit B via (f,g), then any witness for f(x) in B
   can be mapped back to a witness for x in A via g. */
void levin_reduction_demo(void);

/* ===== Completeness Hierarchy (L6, L7, L8) ===== */

/* Completeness beyond NP: demonstrate completeness notions
   for PSPACE, EXP, NEXP, and the polynomial hierarchy. */
void completeness_hierarchy_demo(void);

/* SAT is NP-complete under <=_L (logspace).
   This is the STRONGEST known completeness result for SAT.
   Proof sketch in C with assertion checks. */
void sat_logspace_completeness(void);

#endif
