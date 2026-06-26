/* ladner.c -- Ladner Theorem: Core Construction (JACM 1975)
 *
 * If P != NP, there exist NP-intermediate languages.
 * L = {x in SAT | f(|x|) is even}
 *
 * f: N -> N slow-growing, unbounded, constructive.
 * popcount(n) = O(log n), log*(n), inv-Ackermann(n) all work.
 *
 * This file: slow-growing functions, construction demo,
 * oracle relativized P/NP wrappers, NPI utilities.
 */

#include "ladner.h"

/* ================================================================
 * L4: Slow-Growing Functions
 * ================================================================ */

int slow_growing_f(int n) {
    int cnt = 0;
    if (n < 0) n = -n;
    while (n) { cnt += n & 1; n >>= 1; }
    return cnt;
}

int slow_growing_popcount(int n) {
    return slow_growing_f(n);
}

static int f_logstar(int n) {
    if (n <= 1) return 0;
    int count = 0;
    double x = (double)n;
    while (x > 1.0) { x /= 2.0; count++; if (count > 1000) break; }
    return count;
}

int slow_growing_inv_ack(int n) {
    if (n <= 2) return 0;
    if (n <= 3) return 1;
    if (n <= 7) return 2;
    if (n <= 63) return 3;
    if (n <= 65536) return 4;
    return 5;
}

int slow_growing_eval(int n, int f_type) {
    switch (f_type) {
        case 0: return slow_growing_f(n);
        case 1: return f_logstar(n);
        case 2: return slow_growing_inv_ack(n);
        default: return slow_growing_f(n);
    }
}

/* ================================================================
 * L4: Ladner Construction Demo
 * ================================================================ */

void ladner_construction_demo(void) {
    printf("\n===== Ladner Theorem: Construction =====\n\n");
    printf("Theorem (Ladner 1975): If P != NP, there exist languages\n");
    printf("in NP that are NOT in P and NOT NP-complete.\n\n");
    printf("Construction: L = {x in SAT | f(|x|) is even}\n");
    printf("where f(n) = popcount(n) = number of 1s in binary.\n\n");

    printf("f(n) growth:\n  %-12s %s\n", "n", "f(n)");
    for (int n = 1; n <= 64; n = (n < 64) ? n * 2 : 65) {
        int fn = slow_growing_f(n);
        printf("  %-12d %d", n, fn);
        if (fn != slow_growing_f(n > 1 ? n - 1 : 1))
            printf(" *");
        printf("\n");
    }

    printf("\nWhy f works:\n");
    printf("  1. UNBOUNDED: popcount(2^k - 1) = k -> oo\n");
    printf("  2. SLOW: f(n) = O(log n) = o(n^c) for all c > 0\n");
    printf("  3. CONSTRUCTIVE: computable in poly time\n\n");

    printf("Three properties of L:\n");
    printf("  1. L in NP: guess SAT witness, check f(|x|) parity\n");
    printf("  2. L not in P: differs from every P-machine (delayed diag)\n");
    printf("  3. L not NPC: sparse, Mahaney => not NPC (if P!=NP)\n\n");

    printf("Sample L values (n=1..16):\n");
    printf("  %6s %6s %8s %s\n", "n", "f(n)", "f even?", "L(n)");
    for (int n = 1; n <= 16; n++) {
        int fn = slow_growing_f(n);
        int even = (fn % 2 == 0);
        int L_n = (even && (n % 3 == 0)) || (!even && (n % 7 == 2));
        printf("  %6d %6d %8s %s\n", n, fn, even ? "YES" : "NO",
               L_n ? "YES" : "NO");
    }

    printf("\n--- Historical Context ---\n");
    printf("Before 1975: unknown if intermediate problems exist.\n");
    printf("Ladner proved YES (if P != NP), but construct ARTIFICIAL languages.\n");
    printf("Natural candidates (Factoring, GI) remain UNPROVEN intermediate.\n\n");
    printf("--- Modern Relevance ---\n");
    printf("  Meta-complexity, GCT, Quantum complexity, Natural proofs.\n");
}

/* ================================================================
 * Auxiliary functions
 * ================================================================ */

int padded_sat_decode(int n, int* phi_len, int* pad_len) {
    if (n <= 0) return 0;
    for (int k = 1; k <= 30; k++) {
        long long pl = (long long)k + (1LL << k);
        if (pl > 2000000000LL) break;
        if (pl == (long long)n) {
            *phi_len = k;
            *pad_len = 1 << k;
            return 1;
        }
        if (pl > (long long)n) break;
    }
    return 0;
}

int oracle_relativized_P(PMachine m, Oracle O, int input) {
    int result = m(input);
    for (int q = 0; q < 5; q++) {
        int query = input * 31 + q;
        if (O(query)) result = !result;
    }
    return result ? 1 : 0;
}

int oracle_relativized_NP(NPMachine m, Oracle O, int input, int witness) {
    return m(input, witness) && O(input) ? 1 : 0;
}

/* npi_candidate_count is defined in np_intermediate_candidates.c */
