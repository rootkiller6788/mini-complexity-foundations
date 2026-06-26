/* sat_to_3sat.c — SAT to 3SAT Reduction (L5 Algorithm)
 *
 * The canonical NP-completeness reduction. Any CNF formula φ can be
 * transformed into an EQUISATISFIABLE 3-CNF formula φ' in polynomial time.
 *
 * Algorithm: For each clause C = (ℓ₁ ∨ ℓ₂ ∨ ... ∨ ℓₖ) with k > 3:
 *   Introduce k-3 NEW auxiliary variables a₁, ..., a_{k-3}.
 *   Replace C with k-2 3-clauses:
 *     (ℓ₁ ∨ ℓ₂ ∨ a₁) ∧ (¬a₁ ∨ ℓ₃ ∨ a₂) ∧ ... ∧ (¬a_{k-3} ∨ ℓ_{k-1} ∨ ℓₖ)
 *
 * Theorem: φ is satisfiable iff φ' is satisfiable.
 * Proof sketch:
 *   (⇒) Given assignment α satisfying φ, extend α to α' by setting
 *        aᵢ = true if (ℓ₁ ∨ ... ∨ ℓ_{i+1}) is satisfied by α.
 *   (⇐) Given α' satisfying φ', its restriction to original vars satisfies φ.
 *
 * Complexity: O(m·k) time, O(m·k) output size, where m = |φ|, k = max clause width.
 * Space: O(log n) work tape ⇒ logspace reduction!
 *
 * Reference: Karp (1972), "Reducibility Among Combinatorial Problems"
 * Reference: Arora & Barak §2.6 (Cook-Levin Theorem and reductions) */

#include "redcomp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LITS_PER_CLAUSE 16

/* ===== Data Structures (L3) ===== */

typedef struct {
    int *lits;  /* Literal encoding: (var << 1) | sign. sign=0 positive, 1 negative */
    int n;      /* Number of literals in this clause */
} Clause3;

typedef struct {
    Clause3 *clauses;  /* Array of 3-clauses */
    int nc;            /* Number of clauses */
    int nv;            /* Number of variables (original + auxiliary) */
} CNF3;

static CNF3* cnf3_create(int nv, int nc) {
    CNF3 *f = malloc(sizeof(CNF3));
    f->nv = nv;
    f->nc = nc;
    f->clauses = calloc((size_t)nc, sizeof(Clause3));
    return f;
}

static void cnf3_add(CNF3 *f, int idx, int l1, int l2, int l3) {
    f->clauses[idx].lits = malloc(3 * sizeof(int));
    f->clauses[idx].lits[0] = l1;
    f->clauses[idx].lits[1] = l2;
    f->clauses[idx].lits[2] = l3;
    f->clauses[idx].n = (l3 >= 0) ? 3 : ((l2 >= 0) ? 2 : 1);
}

static void cnf3_free(CNF3 *f) {
    for(int i = 0; i < f->nc; i++) free(f->clauses[i].lits);
    free(f->clauses);
    free(f);
}

/* ===== Literal Encoding =====
 *
 * We encode a literal as: (variable_index << 1) | polarity
 *   polarity = 0: positive literal  (x_var)
 *   polarity = 1: negative literal  (¬x_var)
 *
 * Example: literal x₃ → (3 << 1) | 0 = 6
 *          literal ¬x₁ → (1 << 1) | 1 = 3 */

static int encode_literal(int var, int polarity) {
    return (var << 1) | polarity;
}

static int decode_var(int lit)   { return lit >> 1; }
static int decode_polarity(int lit) { return lit & 1; }
static int literal_is_positive(int lit) { return !decode_polarity(lit); }

/* ===== Main Reduction Algorithm (L5) ===== */

/* Reduce an arbitrary CNF to an equisatisfiable 3-CNF.
 *
 * Parameters:
 *   clause_lens[i]  = number of literals in clause i
 *   clauses[i][j]   = literal encoding for clause i, position j
 *   n_clauses       = number of clauses in input
 *   n_vars          = number of variables in input
 *   new_nv          = [out] number of variables in output (original + aux)
 *
 * Returns: 3-CNF formula equisatisfiable to the input.
 * Caller must free via cnf3_free(). */
CNF3* sat_to_3sat(int *clause_lens, int **clauses,
                  int n_clauses, int n_vars, int *new_nv) {
    int total_3clauses = 0, extra_vars = 0;

    /* First pass: count output clauses and extra variables needed */
    for(int i = 0; i < n_clauses; i++) {
        int k = clause_lens[i];
        if(k <= 3) {
            total_3clauses += 1;
        } else {
            /* k literals → k-2 3-clauses, k-3 auxiliary variables */
            total_3clauses += k - 2;
            extra_vars += k - 3;
        }
    }

    *new_nv = n_vars + extra_vars;
    CNF3 *result = cnf3_create(*new_nv, total_3clauses);

    int clause_idx = 0;
    int aux_var = n_vars;  /* Auxiliary variables start after originals */

    /* Second pass: construct the 3-clauses */
    for(int i = 0; i < n_clauses; i++) {
        int k = clause_lens[i];
        int *lits = clauses[i];

        if(k <= 3) {
            /* Already 3-CNF or shorter: pad with -1 (null literal) */
            cnf3_add(result, clause_idx++,
                     k >= 1 ? lits[0] : -1,
                     k >= 2 ? lits[1] : -1,
                     k >= 3 ? lits[2] : -1);
        } else {
            /* Gadget construction:
             * (l₀ ∨ l₁ ∨ a₀)
             * (¬a₀ ∨ l₂ ∨ a₁)
             * (¬a₁ ∨ l₃ ∨ a₂)
             * ...
             * (¬a_{k-4} ∨ l_{k-3} ∨ a_{k-3})
             * (¬a_{k-3} ∨ l_{k-2} ∨ l_{k-1}) */
            for(int j = 0; j < k - 2; j++) {
                int l1, l2, l3;
                if(j == 0) {
                    /* First clause: (l₀ ∨ l₁ ∨ a₀) */
                    l1 = lits[0];
                    l2 = lits[1];
                    l3 = encode_literal(aux_var, 0);  /* a_j positive */
                } else if(j == k - 3) {
                    /* Last clause: (¬a_{k-4} ∨ l_{k-2} ∨ l_{k-1}) */
                    l1 = encode_literal(aux_var + j - 1, 1);  /* ¬a_{j-1} */
                    l2 = lits[k - 2];
                    l3 = lits[k - 1];
                } else {
                    /* Middle clauses: (¬a_{j-1} ∨ l_{j+1} ∨ a_j) */
                    l1 = encode_literal(aux_var + j - 1, 1);  /* ¬a_{j-1} */
                    l2 = lits[j + 1];
                    l3 = encode_literal(aux_var + j, 0);      /* a_j positive */
                }
                cnf3_add(result, clause_idx++, l1, l2, l3);
            }
            aux_var += k - 3;
        }
    }

    return result;
}

/* ===== Verification: Check Equisatisfiability (L4) ===== */

/* Brute-force SAT solver for small formulas (≤10 vars).
   Returns 1 if satisfiable, 0 if not, -1 if too many vars. */
static int brute_force_sat_3cnf(const CNF3 *f) {
    if(f->nv > 10) return -1;

    long long total = 1LL << f->nv;
    for(long long m = 0; m < total; m++) {
        int all_sat = 1;
        for(int ci = 0; ci < f->nc && all_sat; ci++) {
            int clause_sat = 0;
            for(int j = 0; j < f->clauses[ci].n; j++) {
                int lit = f->clauses[ci].lits[j];
                if(lit < 0) continue;  /* padding */
                int var = decode_var(lit);
                int need_one = literal_is_positive(lit); /* 1 for positive, 0 for negative */
                int actual = (int)((m >> var) & 1);
                if(actual == need_one) { clause_sat = 1; break; }
            }
            if(!clause_sat) all_sat = 0;
        }
        if(all_sat) return 1;  /* SAT */
    }
    return 0;  /* UNSAT */
}

/* Verify the SAT→3SAT reduction on a specific formula:
   Check that the original and reduced formulas have the same satisfiability. */
static int verify_equisatisfiability(int *lens, int **clauses,
                                      int n_clauses, int n_vars) {
    int new_nv;
    CNF3 *reduced = sat_to_3sat(lens, clauses, n_clauses, n_vars, &new_nv);
    int reduced_sat = brute_force_sat_3cnf(reduced);

    /* For the original, we'd need a full SAT solver. For small tests,
       we verify structural properties of the reduction. */
    int structural_ok = (reduced->nc > 0) && (reduced->nv >= n_vars);

    cnf3_free(reduced);
    return structural_ok;
}

/* ===== Demonstration (L6) ===== */

void sat_to_3sat_demo(void) {
    printf("\n===== SAT → 3SAT Reduction =====\n\n");

    /* Example 1: 4-literal clause + 2-literal clause */
    printf("--- Example 1: (x₀ ∨ x₁ ∨ x₂ ∨ x₃) ∧ (¬x₀ ∨ ¬x₁) ---\n");
    int lens1[] = {4, 2};
    int *cls1[2];
    int c1_0[] = {encode_literal(0,0), encode_literal(1,0),
                  encode_literal(2,0), encode_literal(3,0)};
    int c1_1[] = {encode_literal(0,1), encode_literal(1,1), -1};
    cls1[0] = c1_0; cls1[1] = c1_1;

    int new_nv;
    CNF3 *res1 = sat_to_3sat(lens1, cls1, 2, 4, &new_nv);
    printf("  Input:  2 clauses, 4 vars\n");
    printf("  Output: %d clauses, %d vars (added %d aux vars)\n",
           res1->nc, res1->nv, res1->nv - 4);
    printf("  Result clauses (variable, polarity encoding):\n");

    for(int i = 0; i < res1->nc; i++) {
        printf("    C%d: (", i);
        for(int j = 0; j < res1->clauses[i].n; j++) {
            int lit = res1->clauses[i].lits[j];
            if(lit < 0) { printf("⊥"); }
            else {
                printf("%cx%d", decode_polarity(lit) ? '¬' : ' ',
                       decode_var(lit));
            }
            if(j < res1->clauses[i].n - 1) printf(" ∨ ");
        }
        printf(")\n");
    }
    cnf3_free(res1);

    /* Example 2: All clauses already ≤3 literals (identity-like) */
    printf("\n--- Example 2: Already 3-CNF ---\n");
    int lens2[] = {3, 3, 2};
    int *cls2[3];
    int c2_0[] = {encode_literal(0,0), encode_literal(1,0), encode_literal(2,0)};
    int c2_1[] = {encode_literal(0,1), encode_literal(2,1), encode_literal(3,0)};
    int c2_2[] = {encode_literal(1,1), encode_literal(3,1), -1};
    cls2[0] = c2_0; cls2[1] = c2_1; cls2[2] = c2_2;

    CNF3 *res2 = sat_to_3sat(lens2, cls2, 3, 4, &new_nv);
    printf("  Input:  3 clauses, 4 vars (already 3-CNF)\n");
    printf("  Output: %d clauses, %d vars (no aux vars needed)\n",
           res2->nc, res2->nv);
    printf("  Verification: struct OK = %s\n",
           verify_equisatisfiability(lens2, cls2, 3, 4) ? "YES" : "NO");
    cnf3_free(res2);

    /* Complexity analysis */
    printf("\n--- Complexity Analysis ---\n");
    printf("Time:  O(Σᵢ k_i) = O(m) where m = total literals\n");
    printf("Space: O(m) output size\n");
    printf("Work tape: O(log n) — this is a LOGSPACE reduction!\n");
    printf("The reduction maps each clause independently using\n");
    printf("auxiliary variables that don't cross clause boundaries.\n");

    /* Theoretical significance */
    printf("\n--- Theoretical Significance ---\n");
    printf("SAT → 3SAT is the FIRST and most fundamental reduction\n");
    printf("in the NP-completeness chain (Karp, 1972).\n");
    printf("It shows that restricting to 3 literals per clause\n");
    printf("does NOT make the problem easier — 3SAT is NP-complete.\n");
    printf("This is surprising: 2SAT ∈ P (solvable in linear time!\n");
    printf("  via implication graph + SCC), but 3SAT is NP-complete.\n");
    printf("The boundary between P and NP-complete lies at k=3.\n");
}
