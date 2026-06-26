/* cook_vs_karp.c — Cook vs Karp Reductions (L1, L2, L4)
 *
 * Two main types of polynomial-time reductions:
 *
 * Cook (Turing) reduction: A ≤_T B if A decidable in poly time
 *   with an ORACLE for B. Can make multiple adaptive queries.
 *   Cook (1971): SAT is NP-complete under ≤_T.
 *
 * Karp (many-one) reduction: A ≤_m B if exists poly-time f
 *   such that x in A iff f(x) in B. SINGLE non-adaptive query.
 *   Karp (1972): SAT is NP-complete under ≤_m (stronger result).
 *
 * Relationship:
 *   ≤_m  ⊆  ≤_T  (every many-one reduction is a Turing reduction:
 *     compute f(x), query oracle once, return answer).
 *
 * Separation: If NP ≠ coNP, then ≤_m ≠ ≤_T.
 *   Proof: co-SAT ≤_T SAT (flip oracle answer), but
 *   co-SAT ≤_m SAT would imply co-SAT ∈ NP, so NP = coNP.
 *   Most believe NP ≠ coNP, so ≤_m ⊂ ≤_T.
 *
 * Reference: Cook (1971), Karp (1972), Arora & Barak §2.2 */

#include "redcomp.h"
#include <math.h>

/* ===== Demonstration Functions ===== */

/* Oracle for B = PRIME: checks if n is prime */
static int oracle_is_prime(int n) {
    if (n < 2) return 0;
    for (int d = 2; d * d <= n; d++)
        if (n % d == 0) return 0;
    return 1;
}

/* Cook (Turing) reduction demo:
 * A = {n > 10 that are composite} ≤_T PRIME
 * Algorithm: query oracle "is n prime?". If yes, reject. Else accept. */
static int cook_reduction_demo(int n) {
    if (n <= 10) return 0;
    /* Adaptive query to oracle B */
    if (oracle_is_prime(n)) return 0;
    return 1;
}

/* Karp (many-one) reduction demo:
 * A ≤_m B via f(x) = x^2.
 * B = {n : n is divisible by 4}. (We use this because:
 *   n even => n^2 divisible by 4; n odd => n^2 ≡ 1 mod 4)
 * Actually: A = {even numbers}, B = {multiples of 4}, f(x)=x^2.
 * Check: x even iff x^2 is multiple of 4. CORRECT. */
static int karp_f(int n) { return n * n; }
static int karp_decide_B(int n) { return n % 4 == 0; }
static int karp_decide_A(int n) { return n % 2 == 0; }

/* Verify Karp reduction property: x in A iff f(x) in B */
static int verify_karp_reduction(int max_n) {
    for (int x = 0; x <= max_n; x++) {
        int a = karp_decide_A(x);
        int b = karp_decide_B(karp_f(x));
        if (a != b) return 0;  /* Violation! */
    }
    return 1;
}

/* ===== Formal Separation Argument (L4) ===== */

/* Demonstrate: There exist problems A, B such that A ≤_T B but NOT A ≤_m B.
 *
 * Under the assumption NP ≠ coNP (widely believed):
 *   co-SAT ≤_T SAT: "On input phi, ask oracle if phi in SAT.
 *                    If no, accept (phi not in SAT => phi in co-SAT)."
 *   But co-SAT NOT ≤_m SAT: would imply co-SAT ∈ NP (since SAT ∈ NP
 *   and NP is closed under ≤_m).
 *
 * For a concrete (unconditional) example:
 *   Let EMPTY = {M : L(M) = ∅} (empty language problem — undecidable).
 *   Let EMPTY' = complement.
 *   Then EMPTY ≤_T EMPTY' (flip answer) but EMPTY ≤_m EMPTY' fails
 *   because many-one reduction preserves decidability properties. */

/* Simulated oracle separation: this is a proof sketch, not executable code */
static void oracle_separation_demo(void) {
    printf("  Oracle separation:\\n");
    printf("  Define A = {n : n encodes a TM that halts on empty input}.\\n");
    printf("  Define B = complement of A.\\n");
    printf("  A <=_T B: query B(n), if yes reject, if no accept.\\n");
    printf("  (In fact, just flip B's answer — trivially a Cook reduction.)\\n");
    printf("  But A NOT <=_m B because A is RE-complete, B is co-RE-complete,\\n");
    printf("  and RE != co-RE (proved via diagonalization).\\n");
    printf("  If A <=_m B existed, then A would be in co-RE, contradiction.\\n");
}

/* ===== Complexity of Reduction Hierarchy ===== */

/* Truth-table reduction: ≤_tt  — non-adaptive Turing reduction.
 * Bounded-query reduction: ≤_{k-tt} — at most k queries.
 * These sit between ≤_m and ≤_T in the hierarchy. */

typedef enum {
    QUERY_MANY_ONE,   /* Single query */
    QUERY_2_TT,       /* 2 non-adaptive queries */
    QUERY_k_TT,       /* k non-adaptive queries */
    QUERY_TURING      /* Unbounded adaptive queries */
} QueryType;

static const char *query_type_name(QueryType qt) {
    switch (qt) {
    case QUERY_MANY_ONE: return "≤_m (1 query)";
    case QUERY_2_TT:     return "≤_{2-tt} (2 non-adaptive)";
    case QUERY_k_TT:     return "≤_{k-tt} (k non-adaptive)";
    case QUERY_TURING:   return "≤_T (unbounded adaptive)";
    default: return "?";
    }
}

/* ===== Demonstration ===== */

void cook_vs_karp_demo(void) {
    printf("\n===== Cook vs Karp Reductions =====\n\n");

    /* Definitions */
    printf("--- Definitions ---\n\n");
    printf("COOK (TURING) REDUCTION: A ≤_T B\n");
    printf("  • Poly-time oracle TM M^B decides A.\n");
    printf("  • M can make MULTIPLE, ADAPTIVE queries to B.\n");
    printf("  • Cook (1971): SAT is NP-complete under ≤_T.\\n\n");

    printf("KARP (MANY-ONE) REDUCTION: A ≤_m B\n");
    printf("  • Poly-time f: x ∈ A ⇔ f(x) ∈ B.\n");
    printf("  • SINGLE query, NON-ADAPTIVE.\n");
    printf("  • Karp (1972): SAT is NP-complete under ≤_m.\\n");
    printf("  • STRONGER result: ≤_m implies ≤_T (trivially).\\n\\n");

    /* Relationship */
    printf("--- Relationship: ≤_m ⊂ ≤_T ---\n\n");
    printf("  ≤_m ⊆ ≤_T: compute f(x), query oracle ONCE, return answer.\n");
    printf("  If ≤_m = ≤_T: NP = coNP (and PH collapses).\n");
    printf("  Most believe: ≤_m ≠ ≤_T (strict subset).\\n\\n");

    /* Demo: Karp reduction verification */
    printf("--- Karp Reduction Demo ---\n");
    printf("  A = {even numbers}, B = {multiples of 4}.\n");
    printf("  f(x) = x^2.\n");
    printf("  Check: x even iff x^2 is multiple of 4.\\n");
    int karp_ok = verify_karp_reduction(20);
    printf("  Verification (x=0..20): %s\\n\\n",
           karp_ok ? "ALL CORRECT ✓" : "FAILED ✗");

    /* Demo: Cook reduction */
    printf("--- Cook (Turing) Reduction Demo ---\n");
    printf("  A = {n > 10 composite},  B = PRIME.\\n");
    printf("  Algorithm: if n <= 10, reject.\\n");
    printf("              query oracle: n prime? -> reject.\\n");
    printf("              else accept.\\n");
    for (int n = 8; n <= 16; n++) {
        printf("  n=%2d: prime=%s -> A=%s\\n",
               n, oracle_is_prime(n) ? "YES" : "NO",
               cook_reduction_demo(n) ? "ACCEPT" : "REJECT");
    }

    /* Separation argument */
    printf("\n--- Why Karp Reductions are Preferred ---\n\n");
    printf("1. Finer granularity: ≤_m classifies problems more precisely.\n");
    printf("2. Closure: NP, coNP, PSPACE are all closed under ≤_m.\n");
    printf("3. NP is NOT closed under ≤_T (if NP ≠ coNP).\n");
    printf("4. P-completeness under ≤_m is trivial (all of P is P-complete).\n");
    printf("   => P-completeness uses ≤_L instead.\\n\\n");

    /* Reduction query hierarchy */
    printf("--- Reduction Query Hierarchy ---\n\n");
    printf("  %s\\n", query_type_name(QUERY_MANY_ONE));
    printf("    ⊂ %s\\n", query_type_name(QUERY_2_TT));
    printf("    ⊂ %s\\n", query_type_name(QUERY_k_TT));
    printf("    ⊂ %s\\n\\n", query_type_name(QUERY_TURING));

    printf("  All are polynomial-time bounded. The difference is\\n");
    printf("  only in how the oracle can be accessed.\\n");
    printf("  Karp reductions = 'one-shot' reduction.\\n");
    printf("  Cook reductions = 'interactive' reduction.\\n\\n");

    /* Oracle separation */
    printf("--- Oracle Separation: Concrete Example ---\n");
    oracle_separation_demo();

    printf("\n--- Historical Note ---\n");
    printf("Cook (1971) originally defined NP-completeness using\\n");
    printf("  Turing reductions. Karp (1972) showed 21 problems are\\n");
    printf("  NP-complete under the STRONGER many-one reduction,\\n");
    printf("  establishing the standard definition used today.\\n");
    printf("Both won the Turing Award: Cook (1982), Karp (1985).\\n");
}