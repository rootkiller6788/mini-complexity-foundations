/*
 * complexity.h — Complexity Theory: Classes, Bounds, and Properties
 *
 * L1 Definitions:
 *   P:    class of languages decidable by a deterministic TM in polynomial time
 *   NP:   class of languages decidable by a nondeterministic TM in polynomial time
 *   EXP:  class of languages decidable by a deterministic TM in exponential time
 *   NEXP: nondeterministic exponential time
 *
 * L2 Core Concepts:
 *   Polynomial time: O(n^k) for some constant k
 *   Polynomial-time verifier (certificate definition of NP):
 *     L in NP iff there exists a poly-time verifier V such that
 *     x in L <=> exists certificate y with |y| <= poly(|x|) and V(x,y)=1
 *   Polynomial hierarchy: Sigma_k^p, Pi_k^p, Delta_k^p
 *   Oracle machines: relative computability
 *
 * L4 Fundamental Laws:
 *   Time Hierarchy Theorem: P != EXP, NP != NEXP
 *   Savitch's Theorem: NSPACE(s(n)) subseteq DSPACE(s(n)^2)
 *   P subseteq NP subseteq PSPACE = NPSPACE subseteq EXP
 *
 * Reference: Arora-Barak, Sipser, Papadimitriou
 * Courses: MIT 6.045/6.841, Stanford CS254, Berkeley CS172, CMU 15-855
 */

#ifndef COMPLEXITY_H
#define COMPLEXITY_H

#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "sat.h"

/* ================================================================
 * Complexity Class Definitions
 * ================================================================ */

typedef enum {
    CLASS_P,
    CLASS_NP,
    CLASS_CONP,
    CLASS_PSPACE,
    CLASS_NPSPACE,
    CLASS_EXP,
    CLASS_NEXP,
    CLASS_EXPSPACE,
    CLASS_NP_COMPLETE,
    CLASS_PSPACE_COMPLETE,
    CLASS_EXP_COMPLETE,
    CLASS_UNKNOWN
} ComplexityClass;

/* Human-readable class names */
const char* complexity_class_name(ComplexityClass c);

/* ================================================================
 * Polynomial Bounds
 * ================================================================ */

/*
 * Represents a polynomial p(n) = c * n^k.
 * Used to establish running time bounds for algorithms.
 */
typedef struct {
    double coefficient;   /* c */
    double degree;        /* k (may be fractional for sub-polynomial) */
} Polynomial;

/* Evaluate polynomial at n */
double poly_eval(const Polynomial* p, double n);

/* O-notation comparison: returns 1 if p1 = O(p2) asymptotically */
int poly_is_big_o(const Polynomial* p1, const Polynomial* p2);

/* Determine if a function is polynomial (degree not too large) */
int poly_is_reasonable(const Polynomial* p);

/* Common polynomials used in complexity theory */
extern const Polynomial POLY_CONSTANT;    /* O(1) */
extern const Polynomial POLY_LOG;         /* O(log n) */
extern const Polynomial POLY_LINEAR;      /* O(n) */
extern const Polynomial POLY_N_LOG_N;     /* O(n log n) */
extern const Polynomial POLY_QUADRATIC;   /* O(n^2) */
extern const Polynomial POLY_CUBIC;       /* O(n^3) */
extern const Polynomial POLY_EXPONENTIAL; /* O(2^n) — not polynomial! */

/* ================================================================
 * Time Measurement Utilities
 * ================================================================ */

typedef struct {
    clock_t start;
    clock_t elapsed;
    int     running;
} Timer;

void   timer_start(Timer* t);
double timer_elapsed_ms(Timer* t);   /* elapsed in milliseconds */
double timer_elapsed_us(Timer* t);   /* elapsed in microseconds */
void   timer_reset(Timer* t);

/* ================================================================
 * Growth Rate Analysis
 * ================================================================ */

/*
 * Empirically determine the asymptotic growth rate of a function
 * by measuring it at different input sizes and fitting to O(n^k).
 */
typedef struct {
    int*    input_sizes;
    double* times;          /* measured times */
    int     n_points;
    double  fitted_degree;  /* estimated k in O(n^k) */
    double  r_squared;      /* goodness of fit */
} GrowthAnalysis;

/* Measure function f(size) and fit to polynomial */
GrowthAnalysis* growth_analyze(double (*f)(int size),
                                const int* sizes, int n_sizes);
void growth_analysis_free(GrowthAnalysis* ga);
void growth_analysis_print(const GrowthAnalysis* ga);

/* ================================================================
 * Certificate / Verifier Framework (NP definition)
 * ================================================================ */

/*
 * A certificate (witness) for NP languages.
 * For SAT: the certificate is a satisfying assignment.
 * For CLIQUE: the certificate is the set of vertices forming the clique.
 */
typedef struct {
    int*  data;
    int   length;
    int   capacity;
} Certificate;

Certificate* cert_create(int capacity);
void         cert_append(Certificate* cert, int value);
void         cert_free(Certificate* cert);
Certificate* cert_clone(const Certificate* cert);

/*
 * Verifier type: given instance x and certificate y,
 * returns 1 if y proves x in L, 0 otherwise.
 * The verifier must run in polynomial time in |x|.
 */
typedef int (*NPVerifier)(const void* instance, const Certificate* cert);

/*
 * Formal certificate validation:
 * Runs the verifier and checks the certificate length bound.
 * Returns: 1 = valid certificate, 0 = invalid.
 */
int np_verify_certificate(const void* instance, const Certificate* cert,
                           NPVerifier verifier, int cert_length_bound);

/* ================================================================
 * NTM Simulation Bounds (Cook-Levin Prerequisites)
 * ================================================================ */

/*
 * For an NTM running in time T(n), the Cook-Levin tableau
 * requires space P = T(n) + n + 2, where n = |w|.
 * Compute P given n and time bound function.
 */
int cl_compute_space_bound(int input_len, int time_steps);

/*
 * Typical polynomial time bounds for NP machines.
 * p1(n) = n, p2(n) = n^2, p3(n) = n^3, etc.
 */
int poly_bound_linear(int n);
int poly_bound_quadratic(int n);
int poly_bound_cubic(int n);
int poly_bound_nk(int n, int k, int coefficient);

/* ================================================================
 * Self-Reducibility
 * ================================================================ */

/*
 * A language L is self-reducible if there is a polynomial-time
 * oracle algorithm that decides L using an oracle for L on
 * smaller instances.
 *
 * SAT is self-reducible: given a formula phi(x1,...,xn),
 *   phi is SAT => either phi[x1:=0] is SAT or phi[x1:=1] is SAT
 * This property enables search-to-decision reduction.
 */

/*
 * Use a SAT decision oracle to find a satisfying assignment.
 * Makes at most n calls to the oracle.
 * Returns 1 and fills assignment if SAT, 0 if UNSAT.
 */
int sat_self_reduce(const CNF* cnf, Assignment assign,
                     int (*sat_oracle)(const CNF*));

/* ================================================================
 * Complexity Class Relationships
 * ================================================================ */

/* Known containment relationships (all unconditionally true) */
int complexity_p_in_np(void);        /* always returns 1: P subseteq NP */
int complexity_np_in_pspace(void);   /* always returns 1: NP subseteq PSPACE */
int complexity_pspace_in_exp(void);  /* always returns 1: PSPACE subseteq EXP */
int complexity_p_in_exp(void);       /* always returns 1: P subseteq EXP */
int complexity_p_not_equal_exp(void);/* always returns 1: Time Hierarchy */

/* Print the known complexity class diagram */
void complexity_print_diagram(void);

#endif /* COMPLEXITY_H */
