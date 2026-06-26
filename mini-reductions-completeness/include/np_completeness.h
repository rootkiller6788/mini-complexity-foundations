#ifndef NP_COMPLETENESS_H
#define NP_COMPLETENESS_H

#include "redcomp.h"

/* ===== NP-Completeness Core Definitions (L1, L2) =====
 *
 * Definition (Cook 1971, Levin 1973): A language L is NP-complete if
 *   1. L ∈ NP (membership: polynomial-time verifier exists)
 *   2. L is NP-hard: ∀A ∈ NP, A ≤_p L (every NP problem reduces to L)
 *
 * Theorem (Cook-Levin): SAT is NP-complete.
 * Theorem (Karp 1972): 21 combinatorial problems are NP-complete.
 *
 * The significance: if ANY NP-complete problem is in P, then P = NP. */

/* Structure representing a verified NP-completeness result */
typedef struct {
    int problem_id;              /* Problem being proved complete */
    const char *problem_name;    /* e.g., "SAT", "3SAT" */
    int np_membership_verified;  /* 1 if in-NP verified */
    int np_hardness_verified;    /* 1 if NP-hardness verified */
    int reduced_from_id;         /* Which known NPC problem does it reduce from */
    const char *reduction_description;
    int reference_year;          /* Publication year of completeness proof */
    const char *reference_author;/* Who proved it */
} NPCompleteProof;

/* ===== NP Membership Certificates (L3) ===== */

/* An NP certificate/witness. For SAT: satisfying assignment.
   For CLIQUE: k-clique vertices. For VC: k vertices covering all edges. */
typedef struct {
    int *data;        /* Witness data (variable assignments, vertex set, etc.) */
    int length;       /* Length of witness data */
    int is_valid;     /* 1 if witness proves membership */
} Witness;

/* NP verifier function type: checks if witness proves instance ∈ language.
   Must run in time polynomial in |instance|. */
typedef int (*NPVerifier)(const void *instance, const Witness *witness);

/* ===== Hardness and Completeness Lattice (L2, L6) ===== */

/* A completeness result for ANY complexity class */
typedef struct {
    int problem_id;
    const char *problem_name;
    ComplexityClass class;          /* Which class is it complete for? */
    ReductionType reduction_used;   /* Under what reduction type? */
    int is_complete;                /* 1 = complete, 0 = hard only */
    const char *first_proof_ref;    /* First proof reference */
} ClassCompletenessEntry;

/* ===== Hardness Propagation (L4) ===== */

/* If A is C-hard and A ≤ B, then B is C-hard.
   This is the fundamental "hardness propagates upward" principle. */
int hardness_propagates(const DecisionProblem *A, const DecisionProblem *B,
                         ComplexityClass C, ReductionType rtype);

/* If B ∈ C and B is C-complete, then C = co-C iff B complement ∈ C.
   Used to study collapse of complexity classes. */
int completeness_and_collapse(const DecisionProblem *B,
                                ComplexityClass C, int *would_collapse);

/* ===== Fine-Grained Completeness (L8) ===== */

/* Fine-grained reduction: A reduces to B with specific time bounds.
   A ≤_{f(n),g(n)} B means a reduction running in O(f(n)) time,
   producing instances of size O(g(n)). */
typedef struct {
    double time_exp;        /* Time exponent for reduction: O(n^k) */
    double blowup_exp;      /* Instance size blowup: O(n^c) */
    int is_subquadratic;    /* 1 if reduction time is truly subquadratic */
} FineGrainedReduction;

/* Check if a reduction is "fine-grained" (typically subcubic) */
int is_fine_grained(const FineGrainedReduction *fg);

#endif
