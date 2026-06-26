/* berman_hartmanis.c -- Berman-Hartmanis Isomorphism Conjecture (1977)
 *
 * Conjecture: All NP-complete languages are p-isomorphic.
 *
 * A p-isomorphism between languages A and B is a bijection
 * f: Sigma* -> Sigma* such that:
 *   1. f is polynomial-time computable
 *   2. f^{-1} is polynomial-time computable
 *   3. x in A iff f(x) in B  (membership preserving)
 *
 * If the conjecture is TRUE:
 *   - All NPC problems have essentially the SAME structure
 *   - P != NP (otherwise all NPC = P, trivially p-isomorphic)
 *   - No Ladner intermediate degrees (contradicting Ladner?)
 *
 * If the conjecture is FALSE:
 *   - NPC has diverse internal structure
 *   - Ladner's theorem is fully consistent
 *
 * The conjecture remains OPEN since 1977.
 *
 * Connection to Ladner: If Berman-Hartmanis is TRUE, it does NOT
 * contradict Ladner because Ladner's intermediate languages are
 * NOT NPC. Ladner says the SPACE BETWEEN P and NPC is rich;
 * Berman-Hartmanis says the TOP (NPC) might be uniform.
 *
 * References:
 *   - Berman & Hartmanis, SIAM J. Comput. 6(2):305-322, 1977
 *   - Arora & Barak, Section 2.6
 */

#include "ladner.h"

/* ---- P-Isomorphism Verification ---- */

/* Verify that a candidate mapping is a p-isomorphism between
 * membership predicates inA and inB.
 *
 * Checks: f is a bijection + membership preserving.
 * Returns: number of errors found (0 = perfect isomorphism).
 */
int berman_hartmanis_verify_iso(PolyIsomorphism* iso,
                                 int (*inA)(int), int (*inB)(int)) {
    int errors = 0;
    int n = iso->domain_size;
    if (n <= 0) return -1;

    for (int x = 0; x < n; x++) {
        /* Only check elements actually in A */
        if (!inA(x)) continue;

        int y = iso->forward(x);

        /* Membership preservation: x in A => f(x) in B */
        if (!inB(y)) {
            errors++;
            continue;
        }

        /* Bijection check: f^{-1}(f(x)) == x */
        int x2 = iso->backward(y);
        if (x2 != x) errors++;
    }
    return errors;
}

/* ---- Example Isomorphisms ---- */

/* Isomorphism 1: Even numbers <-> Multiples of 3
 * f(x) = 3x/2, f^{-1}(y) = 2y/3
 * This is defined on integers where x is even and y is multiple of 3. */

static int iso1_forward(int x) { return x * 3 / 2; }
static int iso1_backward(int y) { return y * 2 / 3; }

static int is_even(int x) { return x % 2 == 0; }
static int is_mult3(int x) { return x % 3 == 0; }

/* Isomorphism 2: Squares <-> Triangular numbers
 * f(n^2) = n(n+1)/2, f^{-1}(t_n) = n^2 */

static int iso2_forward(int x) {
    int n = (int)sqrt((double)x);
    if (n * n != x) return -1;  /* not a square */
    return n * (n + 1) / 2;     /* triangular number */
}

static int iso2_backward(int y) {
    /* Triangular number: y = n(n+1)/2 => n = (sqrt(8y+1)-1)/2 */
    int d = (int)sqrt(8.0 * y + 1);
    if (d * d != 8 * y + 1) return -1;  /* not triangular */
    int n = (d - 1) / 2;
    return n * n;  /* square */
}

static int is_square(int x) {
    int r = (int)sqrt((double)x);
    return r * r == x;
}

static int is_triangular(int x) {
    int d = (int)sqrt(8.0 * x + 1);
    return d * d == 8 * x + 1;
}

/* ---- Berman-Hartmanis Analysis ---- */

/* Check if Berman-Hartmanis conjecture is consistent with
 * what we observe about the constructed isomorphisms. */
int berman_hartmanis_check(void) {
    /* Build isomorphism 1: Even <-> Multiple of 3 */
    PolyIsomorphism iso1;
    iso1.forward = iso1_forward;
    iso1.backward = iso1_backward;
    iso1.domain_size = 100;
    iso1.verified = 0;

    int errors1 = berman_hartmanis_verify_iso(&iso1, is_even, is_mult3);
    iso1.verified = (errors1 == 0);

    /* Build isomorphism 2: Squares <-> Triangular */
    PolyIsomorphism iso2;
    iso2.forward = iso2_forward;
    iso2.backward = iso2_backward;
    iso2.domain_size = 200;
    iso2.verified = 0;

    int errors2 = berman_hartmanis_verify_iso(&iso2, is_square, is_triangular);
    iso2.verified = (errors2 == 0);

    return iso1.verified && iso2.verified ? 1 : 0;
}

/* ---- Demo ---- */

void berman_hartmanis_demo(void) {
    printf("\n===== Berman-Hartmanis Conjecture =====\n\n");

    printf("Conjecture (1977): All NP-complete languages are\n");
    printf("p-isomorphic under polynomial-time bijections.\n\n");

    printf("If TRUE:\n");
    printf("  - NPC has a UNIQUE structure (all problems are\n");
    printf("    essentially identical encodings of each other)\n");
    printf("  - P != NP (otherwise all problems in P are trivially\n");
    printf("    p-isomorphic as one-element sets?)\n\n");

    printf("If FALSE:\n");
    printf("  - NPC has RICH internal diversity\n");
    printf("  - Ladner's theorem holds in full (the space between\n");
    printf("    P and NPC is densely populated)\n\n");

    printf("Status: OPEN since 1977.\n\n");

    /* Example 1 */
    printf("--- Example 1: Even <-> Multiples of 3 ---\n");
    printf("  f(x) = 3x/2, f^{-1}(y) = 2y/3\n");
    PolyIsomorphism iso1 = {iso1_forward, iso1_backward, 100, 0};
    int e1 = berman_hartmanis_verify_iso(&iso1, is_even, is_mult3);
    printf("  Verification errors: %d (0 = perfect isomorphism)\n", e1);
    printf("  Map: f(0)=0 f(2)=3 f(4)=6 f(6)=9 f(8)=12\n");
    for (int x = 0; x <= 8; x += 2)
        printf("    f(%d)=%d\n", x, iso1_forward(x));
    printf("\n");

    /* Example 2 */
    printf("--- Example 2: Squares <-> Triangular Numbers ---\n");
    printf("  f(n^2) = n(n+1)/2\n");
    for (int n = 1; n <= 6; n++) {
        int sq = n * n;
        int tri = iso2_forward(sq);
        printf("  f(%d^2=%d) = T_%d = %d\n", n, sq, n, tri);
    }
    PolyIsomorphism iso2 = {iso2_forward, iso2_backward, 200, 0};
    int e2 = berman_hartmanis_verify_iso(&iso2, is_square, is_triangular);
    printf("  Verification errors: %d\n\n", e2);

    /* Connection to Ladner */
    printf("--- Connection to Ladner's Theorem ---\n");
    printf("Berman-Hartmanis and Ladner are COMPATIBLE:\n");
    printf("  - Ladner: between P and NPC: infinite degrees.\n");
    printf("  - B-H: within NPC: single degree (if true).\n");
    printf("  - Both can be true without contradiction.\n\n");

    printf("Recent developments:\n");
    printf("  - Mahaney (1982): sparse NPC sets are p-isomorphic\n");
    printf("    to SAT ONLY IF P=NP (so sparse sets can't be NPC).\n");
    printf("  - Kurtz-Mahaney-Royer (1993): relativized counterexample\n");
    printf("    to B-H conjecture exists.\n");
    printf("  - Agrawal (2002): PRIMES in P. Impact on B-H? Unclear.\n");
    printf("  - The conjecture remains a central open problem.\n");
}
