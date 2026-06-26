/* uniqueness_np.c -- Valiant-Vazirani Theorem and Unique-SAT
 *
 * Valiant-Vazirani (TCS 1986): There is a randomized polynomial-time
 * reduction from SAT to Unique-SAT (the problem of deciding if a
 * formula has EXACTLY ONE satisfying assignment).
 *
 * Corollary: If Unique-SAT in P, then NP = RP.
 *
 * The technique: Add random XOR constraints to "isolate" a single
 * solution. With probability >= 1/(8n), exactly one SAT assignment
 * survives the XOR filtering.
 *
 * Connection to Ladner:
 *   - Unique-SAT might be NP-intermediate (not known NPC)
 *   - If US in P => NP = RP (randomized P)
 *   - If US is NPC => SAT <=^p_m US (but US has special structure)
 *   - V-V theorem is one of the great achievements of randomized complexity
 *
 * Implementation: brute-force enumeration of assignments (2^n),
 * V-V hash-based reduction (random XOR constraints).
 */

#include "ladner.h"

/* ---- Solution Counting ---- */

/* Count the number of satisfying assignments for formula f
 * on n_vars variables. Uses brute-force 2^n enumeration. */
int count_solutions(int n_vars, int (*eval)(int)) {
    int count = 0;
    long long total = 1LL << n_vars;
    for (long long assign = 0; assign < total; assign++) {
        if (eval((int)assign)) count++;
    }
    return count;
}

/* ---- Valiant-Vazirani Reduction ---- */

/* Apply random XOR constraints to isolate a single solution.
 * mask:  bitmask selecting which variables participate in XOR
 * tgt:   target parity (0 or 1)
 *
 * Returns: 1 if exactly ONE solution survives, 0 otherwise.
 * On success, *nc = 1. */
int vv_reduce(int n_vars, int (*eval)(int), int* nc) {
    int mask = rand() & ((1 << n_vars) - 1);
    int target = rand() & 1;
    int cnt = 0;

    long long total = 1LL << n_vars;
    for (long long assign = 0; assign < total; assign++) {
        if (!eval((int)assign)) continue;

        /* Compute parity of masked bits */
        int parity = 0;
        for (int v = 0; v < n_vars; v++) {
            if ((mask >> v) & 1)
                parity ^= (assign >> v) & 1;
        }

        if (parity == target) cnt++;
    }
    *nc = cnt;
    return (cnt == 1);
}

/* ---- Valiant-Vazirani: Probabilistic Isolation ---- */

/* Estimate the probability that VV reduction isolates a unique solution.
 * For n vars and s solutions, prob ~ s/(2 * 2^n) for one iteration.
 * Need O(n) independent trials for success prob >= 1/2. */
static double vv_success_prob(int n_vars, int n_solutions) {
    /* For a random XOR constraint, each pair of solutions collides
     * with prob 1/2. So the expected number of isolated solutions is
     * approximately s / 2^{t} for t random constraints. */
    if (n_solutions <= 0) return 0.0;
    double prob = 1.0 / (8.0 * n_vars);
    return (prob > 1.0) ? 1.0 : prob;
}

/* ---- Demo ---- */

void uniqueness_demo(void) {
    printf("\n===== Valiant-Vazirani Theorem =====\n\n");

    printf("Theorem (V-V 1986): SAT <=_r Unique-SAT via randomized\n");
    printf("polynomial-time reduction with success prob >= 1/(8n).\n");
    printf("Corollary: If Unique-SAT in P, then NP = RP.\n\n");

    /* Define some test formulas */
    /* f1: (x1 OR x2 OR x3) -- 7 solutions */
    /* f2: (x1 XOR x2) -- 2 solutions */
    /* f3: (x1 AND x2) -- 1 solution */

    printf("--- Test Formulas ---\n");

    printf("  Brute-force counting for n_vars=3:\n");

    /* Manual counting via a local approach */
    int f1_cnt = 0, f2_cnt = 0, f3_cnt = 0;
    for (int a = 0; a < 8; a++) {
        int x1 = (a >> 0) & 1;
        int x2 = (a >> 1) & 1;
        int x3 = (a >> 2) & 1;

        /* f1: x1 OR x2 OR x3 */
        if (x1 || x2 || x3) f1_cnt++;

        /* f2: x1 XOR x2 */
        if (x1 ^ x2) f2_cnt++;

        /* f3: x1 AND x2 */
        if (x1 && x2) f3_cnt++;
    }
    printf("  f1 (x1 OR x2 OR x3):  %d solutions\n", f1_cnt);
    printf("  f2 (x1 XOR x2):       %d solutions\n", f2_cnt);
    printf("  f3 (x1 AND x2):       %d solutions\n", f3_cnt);

    /* V-V reduction simulation */
    printf("\n--- Valiant-Vazirani Isolation ---\n");
    printf("V-V reduction: add random XOR constraints to filter\n");
    printf("assignments. With nonzero probability, exactly ONE\n");
    printf("satisfying assignment survives.\n\n");

    /* Manual demo with specific masks */
    printf("--- Manual V-V Demo (3 vars) ---\n");
    printf("Formula: x1 OR x2 OR x3 (7 solutions)\n");
    printf("  Mask=001 (XOR x1 with target 0):\n");
    printf("    Surviving: assignments where x1=0\n");
    printf("    These are: x1=0, x2|x3=1 => 3 solutions\n\n");

    printf("  Mask=011 (XOR x1^x2 with target 0):\n");
    printf("    Surviving: assignments where x1==x2 => 4 solutions\n\n");

    printf("  With t constraints, expected survivors = 7/2^t.\n");
    printf("  For t=3: 7/8 < 1 => often 0 or 1 solution.\n\n");

    /* Connection to Ladner */
    printf("--- Ladner Connection ---\n");
    printf("Unique-SAT (US) is a candidate NP-intermediate problem:\n");
    printf("  - US in NP: check assignment is unique + SAT.\n");
    printf("  - If US in P: NP = RP (V-V theorem).\n");
    printf("  - If US is NPC: SAT reduces to US (interesting structure).\n");
    printf("V-V shows US captures the FULL power of NP (modulo randomness).\n");
    printf("Ladner guarantees that if P != NP, NP-intermediate LANGUAGES\n");
    printf("exist. US might be one such language.\n");
}
