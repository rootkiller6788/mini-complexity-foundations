#ifndef COMPLEXITY_CLASSES_H
#define COMPLEXITY_CLASSES_H

/* ── complexity_classes.h — Complexity Class Relations ──
 * L2 Core Concepts: P, NP, EXP, NEXP, 2-EXP, etc.
 * L4 Fundamental Laws: containment relations, hierarchy
 * L6 Canonical Problems: class membership testing
 *
 * Ref: AB §1-3, Sipser §7-9, Papadimitriou §7 */

#include "tht.h"

/* ══════════════════════════════════════════
 * Complexity Class Definitions */

/* Standard class identifiers */
typedef enum {
    CLASS_L,           /* logarithmic space */
    CLASS_NL,          /* nondeterministic logspace */
    CLASS_P,           /* polynomial time */
    CLASS_NP,          /* nondeterministic poly time */
    CLASS_coNP,        /* complement of NP */
    CLASS_PSPACE,      /* polynomial space */
    CLASS_EXP,         /* exponential time */
    CLASS_NEXP,        /* nondeterministic EXP */
    CLASS_EXPSPACE,    /* exponential space */
    CLASS_2EXP,        /* doubly exponential time */
    CLASS_ELEMENTARY,  /* union of all k-EXP */
    CLASS_PR,          /* primitive recursive */
    CLASS_R,           /* recursive (decidable) */
    CLASS_RE,          /* recursively enumerable */
    CLASS_COUNT
} ComplexityClassID;

/* Class descriptor with canonical information */
typedef struct {
    ComplexityClassID id;
    const char *name;
    const char *bound_description;    /* e.g., "O(n^k)" */
    int is_deterministic;
    int is_space_class;              /* 1=space, 0=time */
    const char *canonical_complete_problem;
    const char *containment_status;  /* e.g., "⊊ EXP (PROVED)" */
} ClassDescriptor;

/* ══════════════════════════════════════════
 * Class Descriptor Access */

const ClassDescriptor *cc_get_descriptor(ComplexityClassID id);
int cc_count(void);

/* ══════════════════════════════════════════
 * Containment Relations */

/* Known containment chain (proved):
 * L ⊆ NL ⊆ P ⊆ NP ⊆ PSPACE ⊆ EXP ⊆ NEXP ⊆ EXPSPACE
 * Only one proper inclusion known: L ⊊ PSPACE, P ⊊ EXP, NP ⊊ NEXP. */

/* Test if class_a ⊆ class_b is PROVED.
 * Returns: 1=proved inclusion, 0=open, -1=proved separation */
int cc_containment_known(ComplexityClassID class_a, ComplexityClassID class_b);

/* Print the full complexity zoo containment diagram */
void cc_print_containment_chain(void);

/* ══════════════════════════════════════════
 * Proved Separations via Time Hierarchy */

/* P ⊊ EXP — proved by Hartmanis-Stearns */
int cc_p_not_exp_demonstrate(int n_test);

/* NP ⊊ NEXP — proved via nondeterministic hierarchy */
int cc_np_not_nexp_demonstrate(int n_test);

/* PSPACE ⊊ EXPSPACE — proved via space hierarchy */
int cc_pspace_not_expspace_demonstrate(void);

/* ══════════════════════════════════════════
 * Open Problems */

/* P vs NP: is P = NP? OPEN (Clay Millennium Prize).
 * Show both possibilities and their consequences. */
void cc_p_vs_np_scenarios(void);

/* NP vs coNP: is NP = coNP? OPEN.
 * If NP ≠ coNP then P ≠ NP (contrapositive). */
void cc_np_vs_conp_scenarios(void);

/* PH collapse: if PH has only k levels, PH = Σ_k^P.
 * OPEN: does PH collapse to some finite level? */
void cc_polynomial_hierarchy_scenarios(void);

/* ══════════════════════════════════════════
 * Union Theorems */

/* TIME-union: ∪_k TIME(n^k) = P.
 * Each TIME(n^k) ⊊ TIME(n^{k+1}) but the union is proper. */
void cc_time_union_demonstrate(int max_k, int n);

/* The exponential hierarchy: k-EXP ⊊ (k+1)-EXP for all k.
 * PROVED strict at every level. Unlike PH! */
void cc_exponential_hierarchy_strict(int max_levels, int n);

#endif
