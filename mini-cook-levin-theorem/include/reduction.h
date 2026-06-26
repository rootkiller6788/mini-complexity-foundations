/*
 * reduction.h — Polynomial-Time Reductions & NP-Completeness Framework
 *
 * L2 Core Concepts:
 *   Polynomial-time many-one reduction (Karp reduction):
 *     A <=_p B if there exists a poly-time computable function f
 *     such that for all x: x in A <=> f(x) in B.
 *
 *   Properties:
 *     - Transitivity: if A <=_p B and B <=_p C, then A <=_p C
 *     - If A <=_p B and B in P, then A in P
 *     - If A <=_p B and B in NP, then A in NP
 *
 * L1 Definitions:
 *   NP-Hard: L is NP-hard if for all A in NP, A <=_p L
 *   NP-Complete: L is NP-complete if L in NP and L is NP-hard
 *
 * L4 Fundamental Laws:
 *   Cook-Levin: SAT is NP-complete
 *   Karp (1972): 21 problems are NP-complete via reductions from SAT
 *
 * This module provides:
 *   1. A general reduction framework (function pointer based)
 *   2. Verifiable reduction implementations
 *   3. Chain reduction builder (transitivity)
 *   4. Canonical NP-complete problem reductions
 *
 * Reference: Karp (1972), Sipser section 7.5, Arora-Barak section 2.4
 * Courses: MIT 6.045/6.841, Stanford CS254, Berkeley CS172, CMU 15-855
 */

#ifndef REDUCTION_H
#define REDUCTION_H

#include "sat.h"
#include "cook_levin.h"

/* ================================================================
 * General Reduction Framework
 * ================================================================ */

/*
 * A reduction from problem A to problem B.
 * The transform function maps instance_a -> instance_b.
 * The verify_A function checks if a solution to A is valid.
 * The translate_solution function maps solution_B -> solution_A.
 */
typedef void* (*ReductionTransform)(const void* instance_a);
typedef int   (*SolutionVerifierA)(const void* instance_a, const void* solution_a);
typedef int   (*SolutionVerifierB)(const void* instance_b, const void* solution_b);
typedef void* (*SolutionTranslator)(const void* instance_a,
                                     const void* solution_b);

typedef struct {
    const char*         name;           /* e.g., "SAT -> 3SAT" */
    const char*         problem_a;      /* source problem name */
    const char*         problem_b;      /* target problem name */
    ReductionTransform  transform;
    SolutionVerifierA   verify_a;
    SolutionVerifierB   verify_b;
    SolutionTranslator  translate_back;
    double              poly_degree;    /* n^k bound for transformation */
} Reduction;

/* Validate a reduction: check that x in A <=> f(x) in B for a test set */
int reduction_validate(const Reduction* red,
                        const void** test_instances, int n_tests,
                        void** solutions_a);

/* ================================================================
 * Canonical NP-Complete Reductions
 * ================================================================ */

/* Reduction: SAT -> 3SAT */
typedef struct {
    CNF* sat_formula;
} SATInstance;

typedef struct {
    CNF* formula_3sat;
} ThreeSATInstance;

void* sat_to_3sat_transform(const void* inst);
int   sat_verify_solution(const void* inst, const void* sol);
int   threesat_verify_solution(const void* inst, const void* sol);
void* threesat_to_sat_solution(const void* inst, const void* sol_3sat);

extern const Reduction REDUCTION_SAT_TO_3SAT;

/* Reduction: 3SAT -> CLIQUE */
typedef struct {
    CNF*        formula;
    int         k_clique_size;
} ThreeSATToCliqueInstance;

typedef struct {
    Graph*      graph;
    int         k;
} CliqueInstance;

void* threesat_to_clique_transform(const void* inst);
int   clique_verify_solution(const void* inst, const void* sol);
void* clique_to_threesat_solution(const void* inst, const void* sol_clique);

extern const Reduction REDUCTION_3SAT_TO_CLIQUE;

/* Reduction: 3SAT -> VERTEX-COVER */
typedef struct {
    VCGraph*    graph;
    int         k;
} VertexCoverInstance;

void* threesat_to_vertexcover_transform(const void* inst);
int   vertexcover_verify_solution(const void* inst, const void* sol);
void* vertexcover_to_threesat_solution(const void* inst, const void* sol_vc);

extern const Reduction REDUCTION_3SAT_TO_VERTEXCOVER;

/* ================================================================
 * Reduction Chain (Transitivity)
 * ================================================================ */

/*
 * Compose two reductions: if A <=_p B and B <=_p C, build A <=_p C.
 * The composed transform is f_C . f_B.
 */
Reduction* reduction_compose(const Reduction* a_to_b,
                              const Reduction* b_to_c);

/* Build a chain of reductions, returning the composed reduction */
Reduction* reduction_chain_build(const Reduction** reductions, int count);

/* ================================================================
 * NP-Completeness Certification
 * ================================================================ */

/*
 * Certify that a problem L is NP-complete:
 *   1. L in NP (provide poly-time verifier)
 *   2. SAT <=_p L (provide reduction from SAT)
 */
typedef struct {
    const char*         problem_name;
    int                 (*np_verifier)(const void* instance, const void* certificate);
    const Reduction*    reduction_from_sat;
    int                 verified_np;       /* set after validation */
    int                 verified_hard;     /* set after validation */
} NPCompleteCertificate;

int npc_certificate_validate(NPCompleteCertificate* cert,
                              const void** test_instances, int n_tests);

/* ================================================================
 * Problem Library — Pre-built NP-Complete Problem Instances
 * ================================================================ */

/* SAT instance: (x1 OR x2) AND (NOT x1 OR x3) AND (NOT x2 OR NOT x3) */
CNF* make_sat_example_1(void);

/* SAT instance: (x1 OR x2) AND (NOT x1) AND (NOT x2) — UNSAT */
CNF* make_sat_example_unsat(void);

/* 3SAT instance with 4 variables, 5 clauses */
CNF* make_3sat_example_1(void);

/* ================================================================
 * Reduction Utilities
 * ================================================================ */

/* Print a reduction summary */
void reduction_print(const Reduction* red);

/* Time a reduction transform and return milliseconds */
double reduction_benchmark_transform(const Reduction* red,
                                      const void* instance, int trials);

#endif /* REDUCTION_H */
