/* closure.c — Closure Properties Under Polynomial-Time Reductions (L2, L4)
 *
 * Complexity classes are CLOSED under polynomial-time many-one reductions:
 *   If B ∈ C and A ≤_p B, then A ∈ C.
 *
 * This property is FUNDAMENTAL to complexity theory. It means:
 *   1. If an NP-complete problem is in P, then P = NP.
 *   2. Reductions PRESERVE membership in complexity classes.
 *   3. Completeness is a MEANINGFUL concept because of closure.
 *
 * Closure table:
 *   P        — YES (poly(poly(n)) = poly(n))
 *   NP       — YES (guess f(x), verify with NP machine for B)
 *   coNP     — YES (complement of NP)
 *   PSPACE   — YES (poly(poly(n)) = poly(n) space)
 *   EXP      — YES (2^{poly(n)} = 2^{poly(n)})
 *   NEXP     — YES (same reason)
 *   PH (Σₖ)  — YES (each level closed)
 *   L        — YES (under ≤_L, NOT under ≤_p in general)
 *   NL       — YES (under ≤_L)
 *
 * Reference: Arora & Barak §2.2, §3.1 */

#include "redcomp.h"
#include <math.h>

/* ===== P-Closure Demonstration ===== */

/* Demonstrate: if B ∈ P and A ≤_p B, then A ∈ P.
 *
 * Proof: On input x:
 *   1. Compute y = f(x) in time O(|x|^k).
 *   2. Run the polynomial-time decider for B on y in time O(|y|^c).
 *   3. |y| = O(|x|^k), so step 2 takes O(|x|^{kc}) time.
 *   4. Total = poly(|x|).
 *
 * Example: A = {n : n² is even},  B = {n : n is even},  f(x) = x².
 * B ∈ P (check n%2==0). f is poly-time (multiplication).
 * So A ∈ P — indeed, we can just check n² % 2 == 0. */

static int decider_B_even(int n) { return n % 2 == 0; }
static int reduction_f_square(int n) { return n * n; }
static int decider_A_via_closure(int n) {
    /* A(x) = B(f(x)) = B(x²) = is x² even?
       Equivalently: is x even? (since square preserves parity) */
    return decider_B_even(reduction_f_square(n));
}

/* ===== NP-Closure Demonstration ===== */

/* NP is closed under ≤_p: if B ∈ NP and A ≤_p B, then A ∈ NP.
 *
 * Proof: Given polynomial-time verifier V_B for B and reduction f.
 * Define verifier V_A for A: on input (x, w),
 *   1. Compute y = f(x)  [polynomial time]
 *   2. Run V_B(y, w)     [polynomial time in |y| = poly(|x|)]
 *   3. Accept iff V_B accepts.
 * So V_A runs in polynomial time. ✓ */

/* Simulate an NP verifier for a "virtual" problem B.
   B = {n : exists w < n such that n ≡ w² (mod 17)}.
   Verifier V_B checks if n ≡ w² (mod 17) in poly time. */
static int np_verifier_B(int n, int witness) {
    return (n % 17) == ((witness * witness) % 17);
}

/* A via closure: A ≤_p B with f(x) = 3x + 7.
   Then A ∈ NP via verifier V_A(x, w) = V_B(3x+7, w). */
static int np_verifier_A(int n, int witness) {
    int y = 3 * n + 7;  /* reduction f(x) = 3x+7 */
    return np_verifier_B(y, witness);
}

/* ===== Non-Closure Examples ===== */

/* NOT closed under ≤_p (with polynomial-time TURING reductions):
   If NP ≠ coNP, then NP is NOT closed under ≤_T.
   Reason: co-SAT ≤_T SAT (flip oracle answer), but co-SAT ∉ NP (conjectured). */

/* ===== Self-Reducibility Connection ===== */

/* SAT is SELF-REDUCIBLE: given an oracle for SAT, we can FIND a
   satisfying assignment. This is a Cook/Turing reduction from
   "find assignment" to "SAT decision".

   Self-reducibility is a key property of many NP-complete problems.
   It means the search version ≤_T the decision version. */

/* ===== Formal Closure Proofs (L4) ===== */

/* Prove: If B ∈ P, then A = f^{-1}(B) ∈ P for poly-time f.
   This is a constructive proof: we give the algorithm. */
int closure_P_proof(int (*f)(int), int (*B_decider)(int), int x) {
    /* Algorithm for A(x): compute f(x), then decide B(f(x)).
       Both operations are polynomial-time. */
    return B_decider(f(x));
}

/* Check monotonicity of class containment under reductions */
static int test_P_closure(void) {
    printf("  P-closure test: A(x) = is x² even?\n");
    int all_pass = 1;
    for(int x = 0; x < 20; x++) {
        int expected = (x * x) % 2 == 0;  /* x² even ⇔ x even */
        int result = decider_A_via_closure(x);
        if(expected != result) { all_pass = 0; break; }
    }
    return all_pass;
}

/* Check NP closure */
static int test_NP_closure(void) {
    printf("  NP-closure test: A(x) via verifier composed with reduction.\n");
    int accept_count = 0;
    for(int x = 0; x < 15; x++) {
        int found_witness = 0;
        for(int w = 0; w < x + 5; w++) {
            if(np_verifier_A(x, w)) { found_witness = 1; break; }
        }
        accept_count += found_witness;
    }
    printf("  A accepts %d out of 15 inputs (non-trivial distribution).\n",
           accept_count);
    return accept_count > 0 && accept_count < 15;  /* non-trivial */
}

/* ===== Demonstration ===== */

void closure_demo(void) {
    printf("\n===== Closure Properties Under Reductions =====\n\n");

    printf("Definition: Class C is closed under ≤_p if\n");
    printf("  (B ∈ C and A ≤_p B) ⇒ A ∈ C.\n\n");

    /* Closure table */
    printf("--- Closure Table ---\n\n");
    printf("  Class     Closed under ≤_p?    Reason\n");
    printf("  --------  -----------------    ------\n");
    printf("  P         YES                  poly(poly(n)) = poly(n)\n");
    printf("  NP        YES                  compose reduction + verifier\n");
    printf("  coNP      YES                  complement symmetry\n");
    printf("  PSPACE    YES                  poly(poly(n)) space bound\n");
    printf("  EXP       YES                  2^{poly(n)} closed under poly\n");
    printf("  NEXP      YES                  same as EXP for nondeterminism\n");
    printf("  Σₖ^P       YES                  each PH level closed\n");
    printf("  Πₖ^P       YES                  each PH level closed\n");
    printf("  L          NO (under ≤_p)       poly output may not fit in log space\n");
    printf("  L          YES (under ≤_L)       logspace-closed under logspace red.\n");
    printf("  NL         YES (under ≤_L)       same\n\n");

    /* Crucial implication */
    printf("--- Why Closure Matters: The P vs NP Implication ---\n\n");
    printf("Theorem: If any NP-complete problem L ∈ P, then P = NP.\n");
    printf("Proof:\n");
    printf("  1. L is NP-complete → ∀A ∈ NP, A ≤_p L.\n");
    printf("  2. L ∈ P (assumption).\n");
    printf("  3. By P-closure: A ≤_p L ∈ P ⇒ A ∈ P.\n");
    printf("  4. Therefore NP ⊆ P.\n");
    printf("  5. P ⊆ NP trivially.\n");
    printf("  6. ∴ P = NP. ■\n\n");

    /* Test demonstrations */
    printf("--- Test: P-Closure ---\n");
    printf("  %s\n\n", test_P_closure() ? "PASS ✓" : "FAIL ✗");

    printf("--- Test: NP-Closure ---\n");
    printf("  %s\n\n", test_NP_closure() ? "PASS ✓" : "FAIL ✗");

    /* Non-closure under Turing reductions */
    printf("--- Non-Closure Under Turing Reductions ---\n\n");
    printf("If NP ≠ coNP (widely believed):\n");
    printf("  co-SAT ≤_T SAT  (flip the oracle answer — valid Turing reduction).\n");
    printf("  SAT ∈ NP, but co-SAT ∉ NP.\n");
    printf("  Therefore NP is NOT closed under ≤_T.\n\n");
    printf("This is why NP-completeness uses ≤_m (Karp), not ≤_T (Cook),\n");
    printf("  even though Cook's theorem originally used ≤_T.\n");
    printf("  Karp's result (≤_m) is strictly stronger.\n");
}