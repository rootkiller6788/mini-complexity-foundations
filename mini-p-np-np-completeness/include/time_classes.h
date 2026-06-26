/* time_classes.h — Formal definitions of complexity classes
 *
 * P, NP, coNP, EXP, NEXP, PSPACE, NPSPACE.
 * Each class defined by: resource type (time/space), bound function,
 * machine type (deterministic/nondeterministic).
 *
 * Reference: Arora-Barak §1-§4, Sipser §7
 */
#ifndef TIME_CLASSES_H
#define TIME_CLASSES_H
#include "types.h"

/* Polynomial-time verifier for NP: M(x, certificate) decides in poly(|x|) time.
 * This formalizes: L ∈ NP iff ∃ poly-time verifier V s.t. x∈L ⇔ ∃c, V(x,c)=1 */
typedef int (*Verifier)(const int* input, int input_len, const int* cert, int cert_len);

/* P membership test: simulate a poly-time deterministic TM for the language.
 * Here we model with a function pointer that runs in O(n^k) time. */
typedef int (*PolyTimeDecider)(const int* input, int len);

/* Check if a decider runs in polynomial time (by timing on sample inputs).
 * This is an empirical bound, not a formal proof — used for demos. */
double time_decider(PolyTimeDecider d, const int* input, int len);

/* Check if a problem is in NP: verify it has a poly-time verifier.
 * Given a verifier function, check that:
 *   1. If answer=YES, ∃ cert of poly length s.t. verifier accepts.
 *   2. If answer=NO,  ∀ cert, verifier rejects.
 * This function exhaustively checks (for small inputs). */
int np_membership_test(Verifier v, const int* input, int input_len, int cert_max_len);

/* --- Complexity class enumeration --- */
typedef enum {
    CLASS_P, CLASS_NP, CLASS_CONP,
    CLASS_EXP, CLASS_NEXP,
    CLASS_PSPACE, CLASS_NPSPACE,
    CLASS_PPOLY,          /* P with polynomial advice */
    CLASS_UNKNOWN
} ComplexityClass;

const char* class_name(ComplexityClass c);

/* --- Class inclusion relations (proven theorems) --- */
/* P ⊆ NP ⊆ EXP ⊆ NEXP (deterministic time hierarchy) */
int class_is_subset(ComplexityClass a, ComplexityClass b);

/* Return the smallest known class containing a problem with
   given decider/verifier characteristics */
ComplexityClass classify_problem(int has_poly_decider, int has_poly_verifier,
                                  int has_poly_verifier_co);

/* Print the complexity zoo for the module */
void complexity_zoo_print(void);

/* --- Polynomial-time example deciders for demo --- */
int decider_even(const int* bits, int len);
int decider_balanced(const int* bits, int len);

#endif
