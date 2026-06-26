/* space_diag.h — Diagonalization for Space Hierarchies
 *
 * The Space Hierarchy Theorem: SPACE(f) ⊊ SPACE(g) when f = o(g).
 *
 * Proof technique: Diagonalization.
 *   L_D = {⟨M⟩₁₀ | M on input ⟨M⟩ rejects within g(|⟨M⟩|) space}
 *
 * Unlike Time Hierarchy (which requires O(T log T) overhead),
 * Space Hierarchy requires NO overhead factor because space
 * is reusable — we can clear the work tape and reuse it.
 * This makes the proof simpler.
 *
 * Key lemma: There exists a universal TM U that, given ⟨M⟩₁₀ and x,
 * simulates M(x) using O(s_M(|x|)) space.
 *
 * Reference: Arora & Barak §4.1; Sipser §9.1; Hartmanis-Lewis-Stearns 1965
 */

#ifndef SPACE_DIAG_H
#define SPACE_DIAG_H

#include "spaceh.h"

/* ================================================================
 * L1: Definitions
 * ================================================================ */

/* Space-constructible function g: N → N such that n ↦ bin(g(n))
 * is computable in O(g(n)) space. */
typedef double (*SpaceFn)(double n);

/* Diagonal language: L_D = {1ⁿ | M_n on 1ⁿ rejects within g(n) space} */
typedef struct {
    SpaceFn g_bound;      /* g(n): the outer space bound */
    SpaceFn f_bound;      /* f(n): the inner space bound, f = o(g) */
    int n_input;          /* current input n */
    int diagonal_result;  /* M_n(1ⁿ) result: 1=accept, 0=reject, -1=unknown */
} DiagonalLanguage;

/* ================================================================
 * L2: Core Diagonalization Algorithm
 * ================================================================ */

/* Simulate TM with ID machine_id on input n within space_bound.
 * Returns: 1=accept, 0=reject, -1=exceeds space */
int diag_simulate_tm(int machine_id, int input_n, int space_bound,
                     int max_steps, int* space_used);

/* Run the diagonal language: simulate M_n on 1^n, output ¬result */
int diag_compute(int n, SpaceFn f_bound, SpaceFn g_bound);

/* Universal space-bounded simulation */
int diag_universal_sim(int machine_id, int input_n, int space_bound);

/* ================================================================
 * L3: Diagonal Language Enumeration
 * ================================================================ */

/* Enumerate all space-bounded TMs (Gödel-numbering style) */
typedef struct {
    int machine_id;
    int n_states;
    int tape_alphabet_size;
    SpaceFn space_bound;
    size_t (*space_fn)(size_t);
} TMDescription;

/* Get the n-th TM in a standard enumeration */
TMDescription diag_enum_tm(int n);

/* Count TMs with a given space bound */
int diag_count_tms_with_bound(SpaceFn bound, int max_id);

/* ================================================================
 * L4: Hierarchy Separations
 * ================================================================ */

/* Prove SPACE(n^k) ⊊ SPACE(n^{k+1}) */
void diag_polynomial_separation(int k);

/* Prove L ⊊ PSPACE: SPACE(log n) ⊊ SPACE(n) */
void diag_l_vs_pspace(void);

/* Prove PSPACE ⊊ EXPSPACE */
void diag_pspace_vs_expspace(void);

/* Track the separation gap */
typedef struct {
    const char* class_small;
    const char* class_large;
    SpaceFn f_small;
    SpaceFn g_large;
    double n_at_separation; /* smallest n where separation is shown */
    int verified;           /* 1 if diagonalization verified */
} HierarchySeparation;

/* ================================================================
 * L5: Space-Bounded Universal TM
 * ================================================================ */

/* Universal TM for space-bounded computation.
 * Input: ⟨M⟩ (description of M) + x (M's input).
 * Uses O(s_M(|x|)) space. This is THE key lemma for hierarchy. */
typedef struct {
    int n_tapes;          /* number of work tapes */
    int* tape_heads;
    int* tape_sizes;
    int** tapes;
    int current_state;
    int steps;
} UniversalSpaceTM;

UniversalSpaceTM* utm_create(int description_len);
void utm_free(UniversalSpaceTM* utm);
int utm_initialize(UniversalSpaceTM* utm, const char* machine_desc, int desc_len);
int utm_step(UniversalSpaceTM* utm);
int utm_accepts(UniversalSpaceTM* utm);

/* ================================================================
 * L8: Relativization
 * ================================================================ */

/* Space hierarchy RELATIVIZES: for any oracle A,
 * SPACE^A(f) ⊊ SPACE^A(g) when f=o(g).
 * Unlike P vs NP (which has contradictory relativizations),
 * the space hierarchy is robust to relativization. */
void diag_relativization_demo(void);

/* ================================================================
 * Utilities
 * ================================================================ */

void space_diagonal_demo(void);
int space_bounded_simulate(int machine_id, int input, int space_bound);
void space_hierarchy_demo(void);
int space_hierarchy_diagonal_language(int n);

#endif /* SPACE_DIAG_H */
